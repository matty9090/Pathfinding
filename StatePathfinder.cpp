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
	cam = engine->CreateCamera(tle::kManual, 0.0f, 100.0f, -25.f);

	load_maps();
	load_models();

	scale = settings.getMapScale();
	origin = Vec3<>(scale / 2.0f - (((float)dims.x * scale) / 2.0f), 12.0f, 10.0f);

	map.constructMap(origin, scale);
	displayPath(tree.pathfind_bfs(start, goal));
}

int StatePathfinder::run() {
	while (engine->IsRunning()) {
		if (engine->KeyHit(Key_Escape)) {
			engine->Stop();
			break;
		}

		engine->DrawScene();
	}

	return State::Exit;
}

void StatePathfinder::load_maps() {
	Settings::Map cur_map = settings.getMaps()["Map 1"];
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

	Tree::Node n = tree.findNode(Vec2<>(3, 2));
	cout << "Node " << n->pos.toString() << " costs " << n->cost << endl;
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

void StatePathfinder::displayPath(std::list<Vec2<>> p) {
	Vec2<> prev;

	for (auto coord : p) {
		path.push_back(meshes["Path_BFS"]->CreateModel(coord.x * scale + origin.x, origin.y + 5.4f, coord.y * scale + origin.z));
		path.back()->SetSkin(settings.getModels()["Path_BFS"].tex);
		path.back()->Scale(settings.getModels()["Path_BFS"].scale);

		prev = coord;
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
