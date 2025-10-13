#pragma once

#include "Vertex.h";

#include <vector>

struct MeshData
{
	std::vector<Vertex> vertices;
	std::vector<unsigned int> indices;
};