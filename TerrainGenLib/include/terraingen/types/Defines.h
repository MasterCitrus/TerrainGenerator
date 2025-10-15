#pragma once

#include "TerrainType.h"
#include "Vertex.h"

#include <vector>

enum class NoiseType : uint8_t
{
	Perlin = 0,
	Simplex
};

// Typedefs for Texture data

typedef std::vector<std::vector<float>> HeightData; // Height Map Data
typedef std::vector<Math::Vec3> ColourData;			// Colour Map Data
typedef std::vector<TerrainType> RegionData;		// Region Data
typedef std::vector<unsigned char> TextureData;		// Texture Data

// Typedefs for Mesh data

typedef std::vector<Vertex> VertexData;				// Vertex Data
typedef std::vector<unsigned int> IndexData;		// Vertex Index Data