#include "MapLoader.hpp"

#include <cassert>
#include <fstream>
#include <iostream>

using namespace std;

CMapLoader::CMapLoader()
{

}

// Load the map file and return a multidimensional list of nodes
std::vector<std::vector<int>> CMapLoader::Load(std::string map_file)
{
	// Throw error if width and height aren't valid
	assert(mWidth > 0 && mHeight > 0);

	ifstream mapf(map_file);
	std::vector<std::vector<int>> v;

	if (mapf.is_open())
	{
		// Allocate memory for vector of vectors
		AllocSpace(v);

		// Loop through rows and colums
		for (unsigned y = 0; y < mHeight; ++y)
		{
			for (unsigned x = 0; x <= mWidth; ++x)
			{
				// Read in 1 character (file doesn't use spaces)				
				char c;
				mapf.get(c);

				// Skip end of line character
				if (x >= mWidth)
					continue;
				else
					v[mHeight - y - 1][x] = (int)(c - '0'); // Flip the map vertically to correct it
			}
		}
	}

	mapf.close();

	return v;
}

/* Process the coordinates file */
std::pair<Vec2<>, Vec2<>> CMapLoader::Coords(std::string coords_file)
{
	Vec2<> start, goal;

	ifstream coordf(coords_file);

	if (coordf.is_open())
	{
		int x0, y0, x1, y1;

		// Read in the space separated coordinates
		coordf >> x0 >> y0 >> x1 >> y1;

		start.x = x0, start.y = y0;
		goal.x  = x1, goal.y  = y1;
	}

	coordf.close();

	// Return the start and goal as a pair of 2D coordinates
	return make_pair(start, goal);
}

// Helper function to allocate space for the multidimensional vector
void CMapLoader::AllocSpace(std::vector<std::vector<int>>& v)
{
	v.resize(mHeight);

	for (unsigned i = 0; i < mHeight; ++i)
		v[i].resize(mWidth);
}
