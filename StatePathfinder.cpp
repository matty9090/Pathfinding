#include "StatePathfinder.hpp"
#include "MapLoader.hpp"

#include <cassert>
#include <fstream>

using namespace tle;

StatePathfinder::StatePathfinder(tle::I3DEngine *engine, Settings &settings)
	: State(engine, settings), map(*this), tree(settings.currentMap().dims, map.map)
{
	node_types[0] = make_pair("Wall", 0);
	node_types[1] = make_pair("Clear", 1);
	node_types[2] = make_pair("Wood", 2);
	node_types[3] = make_pair("Water", 3);
}

void StatePathfinder::init() {
	pathNum = 0;
	timer   = 0.f;
	bezier  = true;
	useDiag = false;
	a_found = true, b_found = true;

	for (auto key : settings.getKeys())
		key_list += key.second.desc + "  /  ";

	key_list = key_list.substr(0, key_list.length() - 5);

	cam = engine->CreateCamera(tle::kManual, 0.0f, 100.0f, -25.f);
	font = engine->LoadFont("res/Arial.ttf", 24U);

	load_maps();
	load_models();

	scale = settings.getMapScale();
	origin = Vec3<>(scale / 2.0f - (((float)dims.x * scale) / 2.0f), 12.0f, 10.0f);
	alg = AlgAStar;

	map.constructMap(origin, scale);
}

int StatePathfinder::run() {
	while (engine->IsRunning()) {
		handleInput();

		if (engine->KeyHit(settings.getKeyCode("Main Menu"))) {
			cleanup();
			return State::Menu;
		}

		timer -= engine->Timer();

		if (timer < 0.0f) {
			timer = 0.07f;

			if (!a_found) {
				int r = a_searcher->step();

				if (r == SearchAlgorithm::Found) {
					a_found = true;

					if (bezier)
						displayPathBezier(a_searcher->getPath(), "Path_AS");
					else
						displayPathCatmullRom(a_searcher->getPath(), "Path_AS");

					writeResults(a_searcher->getPath(), "AS");

				} else
					displayPathSearch(a_searcher->getOpenList(), a_searcher->getClosedList(), "_AS");
			}

			if (!b_found) {
				int r = b_searcher->step();

				if (r == SearchAlgorithm::Found) {
					b_found = true;

					if (bezier)
						displayPathBezier(b_searcher->getPath(), "Path_BFS");
					else
						displayPathCatmullRom(b_searcher->getPath(), "Path_BFS");

					writeResults(b_searcher->getPath(), "BFS");
				} else
					displayPathSearch(b_searcher->getOpenList(), b_searcher->getClosedList(), "_BFS");
			}
		}

		displayGUI();
		engine->DrawScene();
	}

	cleanup();

	return State::Exit;
}

void StatePathfinder::load_maps() {
	Settings::Map cur_map = settings.currentMap();
	MapLoader loader;
	pair<Vec2<>, Vec2<>> coords;

	dims = cur_map.dims;
	loader.setDims(dims.x, dims.y);

	auto m = loader.load(settings.getMapsFolder() + cur_map.map_file);
	coords = loader.coords(settings.getMapsFolder() + cur_map.coords_file);

	map.map.resize(dims.y);

	for (unsigned y = 0; y < dims.y; ++y)
		map.map[y].resize(dims.x);

	for (unsigned y = 0; y < dims.y; ++y)
		for (unsigned x = 0; x < dims.x; ++x)
			map.map[y][x] = tree.setNode(x, y, Vec2<>(x, y), m[y][x]);

	start = tree.findNode(coords.first), goal = tree.findNode(coords.second);
}

void StatePathfinder::load_models() {
	auto mesh_list = settings.getModels();

	for (auto mesh : mesh_list) {
		meshes[mesh.first] = engine->LoadMesh(mesh.second.file);

		if (mesh.second.inst) {
			models[mesh.first] = meshes[mesh.first]->CreateModel(mesh.second.pos.x, mesh.second.pos.y, mesh.second.pos.z);

			if(!mesh.second.tex.empty())
				models[mesh.first]->SetSkin(mesh.second.tex);
		}
	}

	assert(meshes.find("Clear") != meshes.end());
	assert(meshes.find("Wood")  != meshes.end());
	assert(meshes.find("Water") != meshes.end());
	assert(meshes.find("Wall")  != meshes.end());

	cam->RotateX(50.0f);
}

void StatePathfinder::free_memory() {
	for (unsigned y = 0; y < dims.y; ++y)
		map.map[y].clear();

	map.map.clear();
}

float StatePathfinder::cspline(float p1, float p2, float p3, float p4, float t) {
	float a = -p1 + 3 * p2 - 3 * p3 + p4;
	float b = 2 * p1 - 5 * p2 + 4 * p3 - p4;
	float c = p3 - p1;
	float d = 2 * p2;

	return (a * t * t * t + b * t * t + c * t + d) / 2.0f;
}

float StatePathfinder::bspline(float p1, float p2, float p3, float p4, float t) {
	return powf(1 - t, 3) * p1 + 3 * t * powf(1 - t, 2) * p2 + 3 * t * t * (1 - t) * p3 + t * t * t * p4;
}

float StatePathfinder::lerp(float v0, float v1, float t) {
	return (1 - t) * v0 + t * v1;
}

void StatePathfinder::handleInput() {
	if (engine->KeyHit(Key_Escape))
		engine->Stop();

	if (engine->KeyHit(settings.getKeyCode("Start"))) {
		clearPathSearch();
		clearPathLine();

		a_found = true, b_found = true;
		pathNum = 0;
		timer = 0.0f;

		if (alg == AlgAStar) a_found = false;
		if (alg == AlgBfs)   b_found = false;
		if (alg == AlgBoth)  a_found = false, b_found = false;

		a_searcher = make_shared<AStar>(tree);
		b_searcher = make_shared<BFS>(tree);

		a_searcher->useDiagonals(useDiag);
		a_searcher->start(start, goal);

		b_searcher->useDiagonals(useDiag);
		b_searcher->start(start, goal);
	}

	if (a_found && b_found && engine->KeyHit(settings.getKeyCode("Clear"))) {
		clearPathSearch();
		clearPathLine();
	}

	if (engine->KeyHit(settings.getKeyCode("Switch Algorithm")))
		alg = (alg + 1) % 3;

	if (engine->KeyHit(settings.getKeyCode("Diagonals")))
		useDiag = !useDiag;

	if (engine->KeyHit(settings.getKeyCode("Curve Type")))
		bezier = !bezier;
}

void StatePathfinder::displayGUI() {
	string alg_str = (alg == AlgBfs) ? "Breadth-First" : ((alg == AlgAStar) ? "A*" : "BFS + A*");
	string diag_str = (useDiag ? "On" : "Off");
	string curv_str = (bezier ? "Bezier" : "Catmul-Rom");

	font->Draw(key_list, 10, 10, kWhite);
	font->Draw("Diagonals: " + diag_str + "  /  " + "Curve: " + curv_str + "  /  " + "Algorithm: " + alg_str, 10, 34, kLightGrey);
}

void StatePathfinder::clearPathSearch() {
	for (auto &i : search_path)
		i->GetMesh()->RemoveModel(i);

	search_path.clear();
}

void StatePathfinder::clearPathLine() {
	for (unsigned i = 0; i < path.size(); i++) {
		for (auto &node : path[i])
			node->GetMesh()->RemoveModel(node);

		path[i].clear();
	}

	path.clear();
}

void StatePathfinder::writeResults(std::vector<Vec2<>> path, string ext) {
	string map_file = settings.currentMap().map_file;
	ofstream file("Results_" + map_file.substr(0, map_file.find_last_of('.')) + "_" + ext + ".txt");

	for (auto coord : path)
		file << coord.toString() << endl;

	file.close();
}

void StatePathfinder::displayPathSearch(std::set<Tree::Node> open, std::set<Tree::Node> closed, string id) {
	for (auto &node : open) {
		int x = node->pos.x, y = node->pos.y;

		search_path.push_back(meshes["OpenList" + id]->CreateModel(x * scale + origin.x, origin.y + 5.0f, y * scale + origin.z));
		search_path.back()->SetSkin(settings.getModels()["OpenList" + id].tex);
		search_path.back()->Scale(settings.getModels()["OpenList" + id].scale);
	}

	for (auto &node : closed) {
		int x = node->pos.x, y = node->pos.y;

		search_path.push_back(meshes["ClosedList" + id]->CreateModel(x * scale + origin.x, origin.y + 5.0f, y * scale + origin.z));
		search_path.back()->SetSkin(settings.getModels()["ClosedList" + id].tex);
		search_path.back()->Scale(settings.getModels()["ClosedList" + id].scale);
	}
}

void StatePathfinder::displayPathBezier(std::vector<Vec2<>> p, string id) {
	if (!p.empty()) {
		path.resize(++pathNum);
		
		int steps = 30;

		for (unsigned i = 0; i < p.size(); i++) {
			Vec2<> ps = (i > 0) ? p[i - 1] : p.front();
			Vec2<> pm = p[i];
			Vec2<> pf = (i < p.size() - 1) ? p[i + 1] : p.back();

			Vec2<float> dir_a = Vec2<float>((float)pm.x, (float)pm.y) - Vec2<float>((float)ps.x, (float)ps.y);
			Vec2<float> dir_b = Vec2<float>((float)pf.x, (float)pf.y) - Vec2<float>((float)pm.x, (float)pm.y);

			// Corner ahead
			if (ps.x != pf.x && ps.y != pf.y) {
				Vec2<float> p1 = Vec2<float>((float)ps.x, (float)ps.y) + dir_a / 2.0f;
				Vec2<float> p2((float)pm.x, (float)pm.y);
				Vec2<float> p3 = p2 + dir_b / 2.0f;
				Vec2<float> p4 = p3;

				for (int n = 0; n < steps; n++) {
					float cx = bspline(p1.x, p2.x, p3.x, p4.x, (float)n / (float)steps);
					float cy = bspline(p1.y, p2.y, p3.y, p4.y, (float)n / (float)steps);

					path[pathNum - 1].push_back(meshes[id]->CreateModel(cx * scale + origin.x, origin.y + 5.0f + (pathNum), cy * scale + origin.z));
					path[pathNum - 1].back()->SetSkin(settings.getModels()[id].tex);
					path[pathNum - 1].back()->Scale(settings.getModels()[id].scale);
				}
			} else {
				for (int n = 0; n < steps; n++) {
					float cx = lerp(pm.x - dir_a.x / 2.0f, pm.x + dir_b.x / 2.0f, (float)n / (float)steps);
					float cy = lerp(pm.y - dir_a.y / 2.0f, pm.y + dir_b.y / 2.0f, (float)n / (float)steps);

					path[pathNum - 1].push_back(meshes[id]->CreateModel(cx * scale + origin.x, origin.y + 5.0f + (pathNum), cy * scale + origin.z));
					path[pathNum - 1].back()->SetSkin(settings.getModels()[id].tex);
					path[pathNum - 1].back()->Scale(settings.getModels()[id].scale);
				}
			}
		}
	}
}

void StatePathfinder::displayPathCatmullRom(std::vector<Vec2<>> p, string id) {
	if (!p.empty()) {
		path.resize(++pathNum);

		int steps = 14;

		for (unsigned i = 0; i < p.size(); i++) {
			Vec2<> p1 = (i > 0) ? p[i - 1] : p[0];
			Vec2<> p2 = p[i];
			Vec2<> p3 = (i < p.size() - 1) ? p[i + 1] : p.back();
			Vec2<> p4 = (i < p.size() - 2) ? p[i + 2] : p.back();

			for (int i = 0; i < steps; i++) {
				float cx = cspline((float)p1.x, (float)p2.x, (float)p3.x, (float)p4.x, (float)i / (float)steps);
				float cy = cspline((float)p1.y, (float)p2.y, (float)p3.y, (float)p4.y, (float)i / (float)steps);

				path[pathNum - 1].push_back(meshes[id]->CreateModel(cx * scale + origin.x, origin.y + 5.0f + (pathNum), cy * scale + origin.z));
				path[pathNum - 1].back()->SetSkin(settings.getModels()[id].tex);
				path[pathNum - 1].back()->Scale(settings.getModels()[id].scale);
			}
		}
	}
}

void StatePathfinder::cleanup() {
	clearPathSearch();
	clearPathLine();

	for (auto mesh : meshes)
		engine->RemoveMesh(mesh.second);

	key_list = "";
	engine->RemoveFont(font);
}

void StatePathfinder::NodeMap::constructMap(Vec3<> origin, float scale) {
	models.resize(parent.dims.y);

	for (unsigned y = 0; y < parent.dims.y; ++y)
		models[y].resize(parent.dims.x);

	for (unsigned y = 0; y < parent.dims.y; ++y) {
		for (unsigned x = 0; x < parent.dims.y; ++x) {
			string type = parent.node_types[map[y][x]->cost].first;
			Vec3<> v = translate(Vec2<>(x, y), origin, scale);

			models[y][x] = parent.meshes[type]->CreateModel(v.x, v.y, v.z);
			models[y][x]->Scale(parent.settings.getModels()[type].scale);

			if(!parent.settings.getModels()[type].tex.empty())
				models[y][x]->SetSkin(parent.settings.getModels()[type].tex);
		}
	}
}

Vec3<> StatePathfinder::NodeMap::translate(Vec2<> coord, Vec3<> &origin, float scale) {
	return Vec3<>(coord.x * scale + origin.x, origin.y, coord.y * scale + origin.z);
}
