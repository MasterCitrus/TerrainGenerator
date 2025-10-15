#pragma once

#include "Defines.h"
#include "math/Vec3.h"

#include <vector>

using namespace Math;

struct MapData
{
	HeightData heightMap;
	ColourData colourMap;

	MapData() = default;

	inline MapData(HeightData heightMap, ColourData colourMap)
		: heightMap(heightMap), colourMap(colourMap) { }
};