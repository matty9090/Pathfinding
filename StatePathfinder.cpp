#include "StatePathfinder.hpp"
#include "MapLoader.hpp"

#include <cassert>

using namespace tle;

StatePathfinder::StatePathfinder(tle::I3DEngine *engine, Settings &settings) : State(engine, settings) {
	
}

void StatePathfinder::init() {
	cam = engine->CreateCamera(tle::kManual, 0.0f, 80.0f, -80.f);

	load_maps();
	load_models();
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

	map = loader.load(settings.getMapsFolder() + cur_map.map_file);
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

	assert(models.find("floor") != models.end());
	cam->RotateX(20.0f);
}

void StatePathfinder::free_memory() {
	for (unsigned y = 0; y < dims.y; ++y)
		map[y].clear();

	map.clear();
}
