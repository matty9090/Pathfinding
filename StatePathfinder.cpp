#include "StatePathfinder.hpp"
#include "MapLoader.hpp"

#include <cassert>

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
	timer = 1.f;
	found = false;
	cam = engine->CreateCamera(tle::kManual, 0.0f, 100.0f, -25.f);

	load_maps();
	load_models();

	scale = settings.getMapScale();
	origin = Vec3<>(scale / 2.0f - (((float)dims.x * scale) / 2.0f), 12.0f, 10.0f);

	map.constructMap(origin, scale);

	searcher = make_shared<BFS>(tree);
	searcher->start(start, goal);

	//displayPath(tree.pathfind_bfs(start, goal), "Path_BFS");
	//displayPath(searcher->getPath(), "Path_AS");
}

int StatePathfinder::run() {
	while (engine->IsRunning()) {
		if (engine->KeyHit(Key_Escape)) {
			engine->Stop();
			break;
		}

		timer -= engine->Timer();

		if (!found && timer < 0.0f) {
			timer = 0.1f;

			int r = searcher->step();

			if (r == SearchAlgorithm::Found) {
				found = true;
				displayPath(searcher->getPath(), "Path_AS");
			} else
				displayPathSearch(searcher->getOpenList(), searcher->getClosedList());
		}

		engine->DrawScene();
	}

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

void StatePathfinder::displayPathSearch(std::set<Tree::Node> open, std::set<Tree::Node> closed) {
	for (auto &i : search_path)
		i->GetMesh()->RemoveModel(i);

	search_path.clear();
	
	for (auto &node : open) {
		int x = node->pos.x, y = node->pos.y;

		search_path.push_back(meshes["OpenList"]->CreateModel(x * scale + origin.x, origin.y + 5.0f, y * scale + origin.z));
		search_path.back()->SetSkin(settings.getModels()["OpenList"].tex);
		search_path.back()->Scale(settings.getModels()["OpenList"].scale);
	}

	for (auto &node : closed) {
		int x = node->pos.x, y = node->pos.y;

		search_path.push_back(meshes["ClosedList"]->CreateModel(x * scale + origin.x, origin.y + 5.0f, y * scale + origin.z));
		search_path.back()->SetSkin(settings.getModels()["ClosedList"].tex);
		search_path.back()->Scale(settings.getModels()["ClosedList"].scale);
	}
}

void StatePathfinder::displayPath(std::vector<Vec2<>> p, string id) {
	if (!p.empty()) {
		path.resize(++pathNum);
		
		int steps = 30;

		for (int i = 0; i < p.size(); i++) {
			Vec2<> ps = (i > 0) ? p[i - 1] : p.front();
			Vec2<> pm = p[i];
			Vec2<> pf = (i < p.size() - 1) ? p[i + 1] : p.back();

			Vec2<float> dir_a = Vec2<float>(pm.x, pm.y) - Vec2<float>(ps.x, ps.y);
			Vec2<float> dir_b = Vec2<float>(pf.x, pf.y) - Vec2<float>(pm.x, pm.y);

			// Corner ahead
			if (ps.x != pf.x && ps.y != pf.y) {
				Vec2<float> p1 = Vec2<float>(ps.x, ps.y) + dir_a / 2.0f;
				Vec2<float> p2(pm.x, pm.y);
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
