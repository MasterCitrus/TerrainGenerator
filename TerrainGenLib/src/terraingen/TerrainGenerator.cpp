#include "terraingen/TerrainGenerator.h"

void TerrainGenerator::GenerateTerrain(GenData* data, const RegionData& regions, float heightMultiplier, unsigned int size)
{
	if (size == 0)
	{
		size = 1;
	}

	MapData mapData;

	mapData = mapGen.GenerateMap(data, regions, size);

	TextureData heightData;
	TextureData colourData;

	heightData = textureGen.GenerateTextureFromNoise(mapData.heightMap);
	colourData = textureGen.GenerateTextureFromColour(mapData.colourMap);

	MeshData meshData;

	meshData = meshGen.GenerateMesh(mapData.heightMap, heightMultiplier);

	this->data.colourTexture = colourData;
	this->data.heightTexture = heightData;
	this->data.meshData = meshData;
	this->data.size = size;
}
