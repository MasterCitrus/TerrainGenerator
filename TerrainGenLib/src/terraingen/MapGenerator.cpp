#include "terraingen/MapGenerator.h"
#include "terraingen/types/math/Utils.h"

#include <random>

MapGenerator::MapGenerator()
{
	perlin = Perlin();
}

MapData MapGenerator::GenerateMap(NoiseType noiseType, const std::vector<TerrainType>& regions, float scale, unsigned int octaves, float persistence, float lacunarity, const Math::Vec2& offset, unsigned int seed)
{
	MapData mapData;

	mapData.heightMap = GenerateNoiseMap(noiseType, scale, octaves, persistence, lacunarity, offset, seed);
	mapData.colourMap = GenerateColourMap(mapData.heightMap, regions);
	mapData.size = mapData.heightMap.size();

	return mapData;
}

Float2D MapGenerator::GenerateNoiseMap(NoiseType noiseType, float scale, unsigned int octaves, float persistence, float lacunarity, const Math::Vec2& offset, unsigned int seed)
{
	if (scale <= 0.0f)
	{
		scale = 0.00001f;
	}

	Float2D map(256, std::vector<float>(256));

	switch (noiseType)
	{
		case NoiseType::Perlin:
			PerlinMap(map, scale, octaves, persistence, lacunarity, offset, seed);
			break;
		case NoiseType::Simplex:

			break;
		default:
			break;
	}
	
	return map;
}

std::vector<Vec3> MapGenerator::GenerateColourMap(const Float2D& map, const std::vector<TerrainType>& regions)
{
	std::vector<Vec3> colourMap(256 * 256);

	for (int y = 0; y < 256; y++)
	{
		for (int x = 0; x < 256; x++)
		{
			float currentHeight = map[y][x];
			for (int i = 0; i < regions.size(); i++)
			{
				if (currentHeight <= regions[i].height)
				{
					colourMap[y * 256 + x] = regions[i].colour;
					break;
				}
			}
			if (regions.empty())
			{
				colourMap[y * 256 + x] = Vec3(0.0f, 0.0f, 0.0f);
			}
		}
	}

	return colourMap;
}

void MapGenerator::PerlinMap(Float2D& map, float scale, unsigned int octaves, float persistence, float lacunarity, const Math::Vec2& offset, unsigned int seed)
{
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
				float sampleX = (x - halfWidth) / scale * frequency + ((octaveOffsets.size() > 0) ? -octaveOffsets[i].x : 0);
				float sampleY = (y - halfHeight) / scale * frequency + ((octaveOffsets.size() > 0) ? octaveOffsets[i].y : 0);

				float value = perlin.Noise(sampleX, sampleY) * 2 - 1;
				noiseHeight += value * amplitude;

				amplitude *= persistence;
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
}
