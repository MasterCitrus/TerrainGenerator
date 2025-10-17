#pragma once

#include <terraingen/types/math/Vec2.h>
#include <terraingen/types/math/Curve.h>
#include <terraingen/types/TerrainType.h>

#include <vector>

enum class FileHeader : uint8_t
{
	TGENV1 = 0,
	FileHeader_COUNT
};

// .tgen File layout
// 
// Header					- 10 bytes
// Num of Regions			- 2 bytes
// Region Data
//	- Region Name length	- 1 byte
//	- Region Name			- variable bytes
//	- Region Colour			- 12 bytes
//	- Region Height			- 4 bytes
// Num of Curve Keys		- 2 bytes
// Curve data				
//	- Curve time			- 4 bytes
//	- Curve value			- 4 bytes
//	- Curve In Tan			- 4 bytes
//  - Curve Out Tan			- 4 bytes
//	- Curve Selected		- 1 byte
// Vec2 offset data			- 8 bytes
// Seed data				- 4 bytes
// Lacunarity data			- 4 bytes
// Persistence data			- 4 bytes
// Noise Scale data			- 4 bytes
// Octave data				- 4 bytes
// Height Multiplier data	- 4 bytes


struct SaveData
{
	std::vector<TerrainType> regions;
	Curve heightCurve;
	Math::Vec2 offset;
	unsigned int seed;
	float lacunarity;
	float persistence;
	float noiseScale;
	unsigned int octaves;
	float heightMultiplier;
};