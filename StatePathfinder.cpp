#include "StatePathfinder.hpp"
#include "MapLoader.hpp"

#include <cassert>

using namespace tle;

StatePathfinder::StatePathfinder(tle::I3DEngine *engine, Settings &settings) : State(engine, settings), map(*this) {
	node_types[0] = make_pair("Wall", 0);
	node_types[1] = make_pair("Clear", 1);
	node_types[2] = make_pair("Wood", 2);
	node_types[3] = make_pair("Water", 3);
}

void StatePathfinder::init() {
	cam = engine->CreateCamera(tle::kManual, 0.0f, 80.0f, -50.f);

	load_maps();
	load_models();

	float scale = settings.getMapScale();
	Vec3<> origin(scale / 2.0f - (((float)dims.x * scale) / 2.0f), 12.0f, (float)dims.y * scale);
	map.constructMap(origin, scale);
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

	map.map = loader.load(settings.getMapsFolder() + cur_map.map_file);
	coords = loader.coords(cur_map.coords_file);

	start = coords.first, goal = coords.second;
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

	cam->RotateX(30.0f);
}

void StatePathfinder::free_memory() {
	for (unsigned y = 0; y < dims.y; ++y)
		map.map[y].clear();

	map.map.clear();
}

void StatePathfinder::NodeMap::constructMap(Vec3<> origin, float scale) {
	models.resize(parent.dims.y);

	for (unsigned y = 0; y < parent.dims.y; ++y)
		models[y].resize(parent.dims.x);

	for (unsigned y = 0; y < parent.dims.y; ++y) {
		for (unsigned x = 0; x < parent.dims.y; ++x) {
			string type = parent.node_types[map[y][x]].first;
			Vec3<> v = translate(Vec2<>(x, y), origin, scale);

			models[y][x] = parent.meshes[type]->CreateModel(v.x, v.y, v.z);
			models[y][x]->Scale(parent.settings.getModels()[type].scale);

			if(!parent.settings.getModels()[type].tex.empty())
				models[y][x]->SetSkin(parent.settings.getModels()[type].tex);
		}
	}
}

Vec3<> StatePathfinder::NodeMap::translate(Vec2<> coord, Vec3<> &origin, float scale) {
	return Vec3<>(coord.x * scale + origin.x, origin.y, -coord.y * scale + origin.z);
}
