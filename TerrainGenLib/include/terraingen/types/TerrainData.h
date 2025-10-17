#pragma once

#include "MapData.h"
#include "MeshData.h"

struct TerrainData
{
	MeshData meshData;
	TextureData heightTexture;
	TextureData noiseTexture;
	TextureData falloffTexture;
	TextureData colourTexture;
	unsigned int size;
	bool usesFalloff;
};