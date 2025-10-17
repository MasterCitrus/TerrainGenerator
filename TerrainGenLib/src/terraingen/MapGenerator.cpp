#include "terraingen/MapGenerator.h"
#include "terraingen/types/math/Utils.h"

#include <random>

MapGenerator::MapGenerator()
{
	perlin = Perlin();
}

MapData MapGenerator::GenerateMap(GenData* data, const RegionData& regions, unsigned int size, bool falloffMap)
{
	MapData mapData;

	mapData.noiseMap = GenerateNoiseMap(data, size);
	if (mapData.falloffMap.empty())
	{
		mapData.falloffMap = GenerateFalloffMap(size);
	}
	mapData.heightMap = CombineNoiseAndFalloff(mapData.noiseMap, mapData.falloffMap, size);
	if(falloffMap)
	{
		mapData.colourMap = GenerateColourMap(mapData.heightMap, regions, size);
	}
	else
	{
		mapData.colourMap = GenerateColourMap(mapData.noiseMap, regions, size);
	}

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

	return HeightData();
}

std::vector<Vec3> MapGenerator::GenerateColourMap(const HeightData& map, const RegionData& regions, unsigned int size)
{
	ColourData colourMap(size * size);

	for (unsigned int y = 0; y < size; y++)
	{
		for (unsigned int x = 0; x < size; x++)
		{
			float currentHeight = map[y][x];
			for (unsigned int i = 0; i < regions.size(); i++)
			{
				if (currentHeight <= regions[i].height)
				{
					colourMap[y * size + x] = regions[i].colour;
					break;
				}
			}
			if (regions.empty())
			{
				colourMap[y * size + x] = Vec3(0.0f, 0.0f, 0.0f);
			}
		}
	}

	return colourMap;
}

HeightData MapGenerator::GenerateFalloffMap(unsigned int size)
{
	HeightData map(size, std::vector<float>(size));

	for (unsigned int i = 0; i < size; i++)
	{
		for (unsigned int j = 0; j < size; j++)
		{
			float x = i / (float)size * 2.0f - 1.0f;
			float y = j / (float)size * 2.0f - 1.0f;

			float value = std::max(std::abs(x), std::abs(y));

			map[j][i] = Evaluate(value);
		}
	}

	return map;
}

HeightData MapGenerator::CombineNoiseAndFalloff(const HeightData& noise, const HeightData& falloff, unsigned int size)
{
	HeightData heightMap(size, std::vector<float>(size));

	for (unsigned int y = 0; y < size; y++)
	{
		for (unsigned int x = 0; x < size; x++)
		{
			float currentHeight = noise[y][x];
			float currentFalloff = falloff[y][x];

			float value = std::max(0.0f, std::min(1.0f, Lerp(currentHeight, -currentFalloff, std::pow(currentFalloff, 1.0f))));
			heightMap[y][x] = value;
		}
	}

	return heightMap;
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

	for (unsigned int y = 0; y < size; y++)
	{
		for (unsigned int x = 0; x < size; x++)
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

	for (unsigned int y = 0; y < size; y++)
	{
		for (unsigned int x = 0; x < size; x++)
		{
			map[y][x] = Math::InverseLerp(minNoiseHeight, maxNoiseHeight, map[y][x]);
		}
	}

	return map;
}

float MapGenerator::Evaluate(float value)
{
	float a = 3.0f;
	float b = 2.2f;

	return std::pow(value, a) / (std::pow(value, a) + std::pow((b - b * value), a));
}
