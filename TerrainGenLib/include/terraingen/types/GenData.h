#pragma once

#include "math/Vec2.h"
#include "Defines.h"

class GenData
{
public:
	GenData(NoiseType type) : type(type) {}

	NoiseType GetType() const { return type; }

protected:
	NoiseType type;

};

class PerlinGenData : public GenData
{
public:
	PerlinGenData(NoiseType type, float scale, unsigned int octaves, float persistence, float lacunarity, const Math::Vec2& offset, unsigned int seed)
		: GenData(type), scale(scale), octaves(octaves), persistence(persistence), lacunarity(lacunarity), offset(offset), seed(seed) { }

	float GetLacunarity() const { return lacunarity; }
	void SetLacunarity(float lacunarity) { this->lacunarity = lacunarity; }
	unsigned int GetOctaves() const { return octaves; }
	void SetOctaves(unsigned int octaves) { this->octaves = octaves; }
	Math::Vec2 GetOffset() const { return offset; }
	void SetOffset(const Math::Vec2& offset) { this->offset = offset; }
	float GetPersistence() const { return persistence; }
	void SetPersistence(float persistence) { this->persistence = persistence; }
	float GetScale() const { return scale; }
	void SetScale(float scale) { this->scale = scale; }
	unsigned int GetSeed() const { return seed; }
	void SetSeed(unsigned int seed) { this->seed = seed; }

private:
	Math::Vec2 offset;
	float scale;
	unsigned int octaves;
	float persistence;
	float lacunarity;
	unsigned int seed;
};