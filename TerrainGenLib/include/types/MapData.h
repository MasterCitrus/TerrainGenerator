#pragma once

#include "math/Vec3.h"

#include <vector>

using namespace Math;

struct MapData
{
	std::vector<float> heightMap;
	std::vector<Vec3> colourMap;

	inline MapData(std::vector<float> heightMap, std::vector<Vec3> colourMap)
		: heightMap(heightMap), colourMap(colourMap) { }
};