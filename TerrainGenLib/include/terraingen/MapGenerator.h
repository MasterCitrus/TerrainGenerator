#pragma once

#include "noise/Perlin.h"
#include "types/Defines.h"
#include "types/GenData.h"
#include "types/MapData.h"

class MapGenerator
{
public:
	MapGenerator();

	MapData GenerateMap(GenData* data, const RegionData& regions, unsigned int size);

private:
	HeightData GenerateNoiseMap(GenData* data, unsigned int size);
	ColourData GenerateColourMap(const HeightData& map, const RegionData& regions);

	HeightData PerlinMap(PerlinGenData data, unsigned int size);

private:
	Perlin perlin;
};