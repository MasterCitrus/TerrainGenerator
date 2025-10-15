#pragma once

#include "MapGenerator.h"
#include "MeshGenerator.h"
#include "types/TerrainData.h"
#include "TextureGenerator.h"

class TerrainGenerator
{
public:
	TerrainGenerator() = default;

	void GenerateTerrain(GenData* data, const RegionData& regions, float heightMultiplier = 1.0f, unsigned int size = 256);

	TerrainData GetData() const { return data; }

private:
	TerrainData data;
	MapGenerator mapGen;
	MeshGenerator meshGen;
	TextureGenerator textureGen;
};