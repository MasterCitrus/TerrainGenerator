#pragma once

#include "noise/Perlin.h"
#include "types/math/Vec2.h"

#include <vector>

enum class NoiseType : uint8_t
{
	Perlin = 0,
	Simplex,
};

typedef std::vector<std::vector<float>> Float2D;

class MapGenerator
{
public:
	MapGenerator();

	Float2D GenerateNoiseMap(NoiseType noiseType, float scale, unsigned int octaves, float persistence, float lacunarity, const Math::Vec2& offset, unsigned int seed = std::default_random_engine::default_seed);
private:
	void GenerateColourMap();

	void PerlinMap(Float2D& map, float scale, unsigned int octaves, float persistence, float lacunarity, const Math::Vec2& offset, unsigned int seed);

private:
	Perlin perlin;
};