#pragma once

#include <TL-Engine.h>

class App {
	public:
		App(size_t w, size_t h);
		~App();

		void run();

	private:
		tle::I3DEngine *eng;
};