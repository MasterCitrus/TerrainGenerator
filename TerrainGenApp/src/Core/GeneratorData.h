#pragma once

#include "terraingen/types/math/Vec2.h"
#include <cstdint>

enum class DisplayType : uint8_t
{
	Quad = 0,
	Mesh,
};

enum class DisplayTextureType : uint8_t
{
	HeightMap = 0,
	ColourMap,
};

extern const char* displayTypeNames[2];
extern const char* displayTextureTypeNames[2];
extern const char* noiseTypeNames[2];

struct GeneratorData
{
	unsigned int seed = 0;
	float lacunarity = 2.0f;
	float persistence = 0.5f;
	float noiseScale = 20.0f;
	unsigned int octaves = 1;
	Math::Vec2 offset = { 0.0f, 0.0f };
	float heightMultiplier = 1.0f;

	bool operator==(const GeneratorData& other)
	{
		return this->seed == other.seed
			&& this->lacunarity == other.lacunarity
			&& this->persistence == other.persistence
			&& this->noiseScale == other.noiseScale
			&& this->octaves == other.octaves
			&& this->offset == other.offset;
	}
	bool operator!=(const GeneratorData& other)
	{
		return !(*this == other);
	}
};