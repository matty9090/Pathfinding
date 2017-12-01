#include "StatePathfinder.hpp"
#include "MapLoader.hpp"

using namespace tle;

StatePathfinder::StatePathfinder(tle::I3DEngine *engine, Settings &settings) : State(engine, settings) {
	
}

void StatePathfinder::init() {
	Settings::Map cur_map = settings.getMaps()["Map 1"];
	MapLoader loader;
	pair<Vec2<>, Vec2<>> coords;

	dims = cur_map.dims;
	loader.setDims(dims.x, dims.y);
	
	map = loader.load(settings.getMapsFolder() + cur_map.map_file);
	coords = loader.coords(cur_map.coords_file);

	start = coords.first, goal = coords.second;

	for (unsigned y = 0; y < cur_map.dims.y; ++y) {
		for (unsigned x = 0; x < cur_map.dims.x; ++x) {
			cout << map[y][x] << " ";
		}

		cout << "\n";
	}
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

void StatePathfinder::free_memory() {

}
