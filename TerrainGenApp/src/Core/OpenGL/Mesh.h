#pragma once

#include "Vertex.h"

#include <vector>

class Material;

class Mesh
{
public:
	Mesh(const std::vector<Vertex>& vertices, const std::vector<unsigned int> indices);

	void Draw();

private:
	std::vector<Vertex> vertices;
	std::vector<unsigned int> indices;
	Material* texture;
};