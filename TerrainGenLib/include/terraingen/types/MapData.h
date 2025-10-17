#pragma once

#include "Defines.h"
#include "math/Vec3.h"

#include <vector>

using namespace Math;

struct MapData
{
	HeightData noiseMap;
	HeightData falloffMap;
	HeightData heightMap;
	ColourData colourMap;

	MapData() = default;

	inline MapData(HeightData heightMap, HeightData noiseMap, HeightData falloffMap, ColourData colourMap)
		: heightMap(heightMap), noiseMap(noiseMap), falloffMap(falloffMap), colourMap(colourMap) { }
};