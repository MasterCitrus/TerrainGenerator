#include "terraingen/MapGenerator.h"
#include "terraingen/types/math/Utils.h"

#include <random>

MapGenerator::MapGenerator()
{
	perlin = Perlin();
}

MapData MapGenerator::GenerateMap(GenData* data, const RegionData& regions, unsigned int size)
{
	MapData mapData;

	mapData.heightMap = GenerateNoiseMap(data, size);
	mapData.colourMap = GenerateColourMap(mapData.heightMap, regions);

	return mapData;
}

HeightData MapGenerator::GenerateNoiseMap(GenData* data, unsigned int size)
{
	switch (data->GetType())
	{
		case NoiseType::Perlin:
		{
			PerlinGenData* perlin = static_cast<PerlinGenData*>(data);
			return PerlinMap(*perlin, size);
			break;
		}
		case NoiseType::Simplex:

			break;
		default:
			break;
	}
}

std::vector<Vec3> MapGenerator::GenerateColourMap(const HeightData& map, const RegionData& regions)
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

HeightData MapGenerator::PerlinMap(PerlinGenData data, unsigned int size)
{
	float scale = data.GetScale();

	if (scale < 0.0f) scale = 0.00001f;

	HeightData map(size, std::vector<float>(size));

	std::default_random_engine gen(data.GetSeed());
	std::uniform_real_distribution<float> dist(-100000.0, 100000.0);

	std::vector<Math::Vec2> octaveOffsets;

	for (unsigned int i = 0; i < data.GetOctaves(); i++)
	{
		float offsetX = dist(gen) + data.GetOffset().x;
		float offsetY = dist(gen) + data.GetOffset().y;
		octaveOffsets.push_back(Math::Vec2(offsetX, offsetY));
	}

	float maxNoiseHeight = FLT_MIN;
	float minNoiseHeight = FLT_MAX;

	float halfWidth = size / 2.0f;
	float halfHeight = size / 2.0f;

	for (int y = 0; y < size; y++)
	{
		for (int x = 0; x < size; x++)
		{
			float amplitude = 1.0f;
			float frequency = 1.0f;
			float noiseHeight = 0.0f;

			for (unsigned int i = 0; i < data.GetOctaves(); i++)
			{
				float sampleX = (x - halfWidth) / scale * frequency + ((octaveOffsets.size() > 0) ? -octaveOffsets[i].x : 0);
				float sampleY = (y - halfHeight) / scale * frequency + ((octaveOffsets.size() > 0) ? octaveOffsets[i].y : 0);

				float value = perlin.Noise(sampleX, sampleY) * 2 - 1;
				noiseHeight += value * amplitude;

				amplitude *= data.GetPersistence();
				frequency *= data.GetLacunarity();
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

	for (int y = 0; y < size; y++)
	{
		for (int x = 0; x < size; x++)
		{
			map[y][x] = Math::InverseLerp(minNoiseHeight, maxNoiseHeight, map[y][x]);
		}
	}

	return map;
}
