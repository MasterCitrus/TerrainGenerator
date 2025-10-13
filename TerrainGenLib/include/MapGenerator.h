#pragma once

#include "noise/Perlin.h"
#include "types/math/Vec2.h"

#include <vector>

typedef std::vector<std::vector<float>> Float2D;

class MapGenerator
{
public:
	MapGenerator();

	Float2D GenerateNoiseMap(float scale, unsigned int octaves, float persistance, float lacunarity, const Math::Vec2& offset, unsigned int seed = std::default_random_engine::default_seed);
private:
	void GenerateColourMap();


private:
	Perlin perlin;
};