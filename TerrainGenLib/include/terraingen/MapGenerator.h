#pragma once

#include "noise/Perlin.h"
#include "types/Defines.h"
#include "types/GenData.h"
#include "types/MapData.h"

class MapGenerator
{
public:
	MapGenerator();

	MapData GenerateMap(GenData* data, const RegionData& regions, unsigned int size, bool falloffMap);

private:
	HeightData GenerateNoiseMap(GenData* data, unsigned int size);
	ColourData GenerateColourMap(const HeightData& map, const RegionData& regions, unsigned int size);
	HeightData GenerateFalloffMap(unsigned int size);
	HeightData CombineNoiseAndFalloff(const HeightData& noise, const HeightData& falloff, unsigned int size);

	HeightData PerlinMap(PerlinGenData data, unsigned int size);

private:

	float Evaluate(float value);

private:
	Perlin perlin;
};