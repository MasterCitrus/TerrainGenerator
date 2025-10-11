#pragma once

#include "Vertex.h"

#include <vector>

class Material;

enum class MeshShape : uint8_t
{
	Quad,
	Cube,
	UVSphere,
	IcoSphere
};

class Mesh
{
public:
	Mesh() = default;
	Mesh(Material* material, MeshShape shape = MeshShape::Cube);
	Mesh(const std::vector<Vertex>& vertices, const std::vector<unsigned int> indices, Material* material);
	~Mesh();

	Material* GetMaterial() { return material; }
	void SetMaterial(Material* material) { this->material = material; }

	void Draw();

private:
	void MakeQuad();
	void MakeCube();
	void MakeUVSphere();
	void MakeIcoSphere();

private:
	std::vector<Vertex> vertices;
	std::vector<unsigned int> indices;
	Material* material = nullptr;
	unsigned int VAO, VBO, IBO;
};