#pragma once

#include "types/math/Vec2.h"

struct GeneratorData
{
	unsigned int seed = 0;
	float lacunarity = 2.0f;
	float persistence = 0.5f;
	float noiseScale = 20.0f;
	unsigned int octaves = 1;
	Math::Vec2 offset = { 0.0f, 0.0f };

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