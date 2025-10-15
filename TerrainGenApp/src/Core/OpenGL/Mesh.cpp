#include "Mesh.h"
#include "Material.h"

#include <glad/glad.h>

using namespace OpenGL;

Mesh::Mesh(Material* material, MeshShape shape)
	: material(material)
{
	switch (shape)
	{
		case MeshShape::Quad:
			MakeQuad();
			break;
		case MeshShape::Cube:
			MakeCube();
			break;
		case MeshShape::UVSphere:
			MakeUVSphere();
			break;
		case MeshShape::IcoSphere:
			MakeIcoSphere();
			break;
		default:
			break;
	}

	glCreateVertexArrays(1, &VAO);
	glCreateBuffers(1, &VBO);
	glCreateBuffers(1, &IBO);

	glNamedBufferData(VBO, sizeof(Vertex) * this->vertices.size(), this->vertices.empty() ? nullptr : this->vertices.data(), GL_STATIC_DRAW);
	glNamedBufferData(IBO, sizeof(unsigned int) * this->indices.size(), this->indices.empty() ? nullptr : this->indices.data(), GL_STATIC_DRAW);

	glVertexArrayVertexBuffer(VAO, 0, VBO, 0, sizeof(Vertex));
	glVertexArrayElementBuffer(VAO, IBO);

	glEnableVertexArrayAttrib(VAO, 0);
	glVertexArrayAttribFormat(VAO, 0, 3, GL_FLOAT, GL_FALSE, offsetof(Vertex, position));
	glVertexArrayAttribBinding(VAO, 0, 0);
	glEnableVertexArrayAttrib(VAO, 1);
	glVertexArrayAttribFormat(VAO, 1, 3, GL_FLOAT, GL_FALSE, offsetof(Vertex, normal));
	glVertexArrayAttribBinding(VAO, 1, 0);
	glEnableVertexArrayAttrib(VAO, 2);
	glVertexArrayAttribFormat(VAO, 2, 2, GL_FLOAT, GL_FALSE, offsetof(Vertex, uv));
	glVertexArrayAttribBinding(VAO, 2, 0);
}

Mesh::Mesh(const std::vector<Vertex>& vertices, const std::vector<unsigned int> indices, Material* material)
	: vertices(vertices), indices(indices), material(material)
{
	glCreateVertexArrays(1, &VAO);
	glCreateBuffers(1, &VBO);
	glCreateBuffers(1, &IBO);

	glNamedBufferData(VBO, sizeof(Vertex) * this->vertices.size(), this->vertices.empty() ? nullptr : this->vertices.data(), GL_STATIC_DRAW);
	glNamedBufferData(IBO, sizeof(unsigned int) * this->indices.size(), this->indices.empty() ? nullptr : this->indices.data(), GL_STATIC_DRAW);

	glVertexArrayVertexBuffer(VAO, 0, VBO, 0, sizeof(Vertex));
	glVertexArrayElementBuffer(VAO, IBO);

	glEnableVertexArrayAttrib(VAO, 0);
	glVertexArrayAttribFormat(VAO, 1, 3, GL_FLOAT, GL_FALSE, offsetof(Vertex, position));
	glVertexArrayAttribBinding(VAO, 0, 0);
	glEnableVertexArrayAttrib(VAO, 1);
	glVertexArrayAttribFormat(VAO, 1, 3, GL_FLOAT, GL_FALSE, offsetof(Vertex, normal));
	glVertexArrayAttribBinding(VAO, 1, 0);
	glEnableVertexArrayAttrib(VAO, 2);
	glVertexArrayAttribFormat(VAO, 2, 2, GL_FLOAT, GL_FALSE, offsetof(Vertex, uv));
	glVertexArrayAttribBinding(VAO, 2, 0);
}

Mesh::~Mesh()
{
	glDeleteVertexArrays(1, &VAO);
	glDeleteBuffers(1, &VBO);
	glDeleteBuffers(1, &IBO);
}

void Mesh::MakeQuad()
{
	// Vertex positions
	glm::vec3 pos1 = { -0.5f, 0.0f, -0.5f }; // Bottom Left
	glm::vec3 pos2 = { 0.5f, 0.0f, -0.5f }; // Bottom Right
	glm::vec3 pos3 = { -0.5f, 0.0f, 0.5f }; // Top Left
	glm::vec3 pos4 = { 0.5f, 0.0f, 0.5f }; // Top Right

	glm::vec3 norm = { 0.0f, 1.0f, 0.0f }; // Y up

	glm::vec2 uv1 = { 0.0f, 0.0f }; // Bottom Left
	glm::vec2 uv2 = { 1.0f, 0.0f }; // Bottom Right
	glm::vec2 uv3 = { 0.0f, 1.0f }; // Top Left
	glm::vec2 uv4 = { 1.0f, 1.0f }; // Top Right

	Vertex bottomLeft = { pos1, norm, uv1 };
	Vertex bottomRight = { pos2, norm, uv2 };
	Vertex topRight = { pos3, norm, uv3 };
	Vertex topLeft = { pos4, norm, uv4 };

	vertices.push_back(bottomLeft);
	vertices.push_back(bottomRight);
	vertices.push_back(topLeft);
	vertices.push_back(topRight);

	indices = { 0, 1, 2, 2, 3, 0 };
}

void Mesh::MakeCube()
{
}

void Mesh::MakeUVSphere()
{
}

void Mesh::MakeIcoSphere()
{
}

void Mesh::Draw()
{
	material->Apply();
	glBindVertexArray(VAO);
	glDrawElements(GL_TRIANGLES, indices.size(), GL_UNSIGNED_INT, nullptr);
	glBindVertexArray(0);
}
