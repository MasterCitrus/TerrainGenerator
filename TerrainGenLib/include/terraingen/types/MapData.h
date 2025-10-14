#pragma once

#include "math/Vec3.h"

#include <vector>

using namespace Math;

typedef std::vector<std::vector<float>> Float2D;

struct MapData
{
	Float2D heightMap;
	std::vector<Vec3> colourMap;
	unsigned int size;

	MapData() = default;

	inline MapData(Float2D heightMap, std::vector<Vec3> colourMap)
		: heightMap(heightMap), colourMap(colourMap) { }
};