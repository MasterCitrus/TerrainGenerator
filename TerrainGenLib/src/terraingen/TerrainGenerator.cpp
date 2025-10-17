#include "terraingen/TerrainGenerator.h"

void TerrainGenerator::GenerateTerrain(GenData* data, const RegionData& regions, const Curve& heightCurve, bool falloffMap, float heightMultiplier, unsigned int size)
{
	if (size == 0)
	{
		size = 1;
	}

	MapData mapData;

	if (falloffMap)
	{
		mapData = mapGen.GenerateMap(data, regions, size, true);
	}
	else
	{
		mapData = mapGen.GenerateMap(data, regions, size, false);
	}

	TextureData heightData;
	TextureData noiseData;
	TextureData falloffData;
	TextureData colourData;

	heightData = textureGen.GenerateTextureFromNoise(mapData.heightMap);
	noiseData = textureGen.GenerateTextureFromNoise(mapData.noiseMap);
	falloffData = textureGen.GenerateTextureFromNoise(mapData.falloffMap);
	colourData = textureGen.GenerateTextureFromColour(mapData.colourMap);

	this->data.colourTexture = colourData;
	this->data.heightTexture = heightData;
	this->data.noiseTexture = noiseData;
	this->data.falloffTexture = falloffData;

	MeshData meshData;

	if (falloffMap)
	{
		meshData = meshGen.GenerateMesh(mapData.heightMap, heightMultiplier, heightCurve);
		this->data.usesFalloff = true;
	}
	else
	{
		meshData = meshGen.GenerateMesh(mapData.noiseMap, heightMultiplier, heightCurve);
		this->data.usesFalloff = false;
	}
	
	this->data.meshData = meshData;
	this->data.size = size;
}
