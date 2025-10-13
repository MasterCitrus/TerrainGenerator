#include "MapGenerator.h"
#include "types/math/Utils.h"

#include <random>

MapGenerator::MapGenerator()
{
	perlin = Perlin();
}

Float2D MapGenerator::GenerateNoiseMap(float scale, unsigned int octaves, float persistance, float lacunarity, const Math::Vec2& offset, unsigned int seed)
{
	if (scale <= 0.0f)
	{
		scale = 0.00001f;
	}

	Float2D map(256, std::vector<float>(256));

	std::default_random_engine gen(seed);
	std::uniform_real_distribution<float> dist(-100000.0, 100000.0);

	std::vector<Math::Vec2> octaveOffsets;

	for (unsigned int i = 0; i < octaves; i++)
	{
		float offsetX = dist(gen) + offset.x;
		float offsetY = dist(gen) + offset.y;
		octaveOffsets.push_back(Math::Vec2(offsetX, offsetY));
	}

	float maxNoiseHeight = FLT_MIN;
	float minNoiseHeight = FLT_MAX;

	float halfWidth = 256.0f / 2.0f;
	float halfHeight = 256.0f / 2.0f;

	for (int y = 0; y < 256; y++)
	{
		for (int x = 0; x < 256; x++)
		{
			float amplitude = 1.0f;
			float frequency = 1.0f;
			float noiseHeight = 0.0f;

			for (unsigned int i = 0; i < octaves; i++)
			{
				float sampleX = (x - halfWidth) / scale * frequency + ((octaveOffsets.size() > 0) ? octaveOffsets[i].x : 0);
				float sampleY = (y - halfHeight) / scale * frequency + ((octaveOffsets.size() > 0) ? octaveOffsets[i].y : 0);

				float value = perlin.Noise(sampleX, sampleY) * 2 - 1;
				noiseHeight += value * amplitude;

				amplitude *= persistance;
				frequency *= lacunarity;
			}

			if (noiseHeight > maxNoiseHeight)
			{
				maxNoiseHeight = noiseHeight;
			}
			else if (noiseHeight < minNoiseHeight)
			{
				minNoiseHeight = noiseHeight;
			}
			map[y][x] = noiseHeight;
		}
	}

	for (int y = 0; y < 256; y++)
	{
		for (int x = 0; x < 256; x++)
		{
			map[y][x] = Math::InverseLerp(minNoiseHeight, maxNoiseHeight, map[y][x]);
		}
	}

	return map;
}
