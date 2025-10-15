#pragma once

#include "MapData.h"
#include "MeshData.h"

struct TerrainData
{
	MeshData meshData;
	TextureData heightTexture;
	TextureData colourTexture;
	unsigned int size;
};