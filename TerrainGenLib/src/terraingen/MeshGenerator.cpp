#include "terraingen/MeshGenerator.h"
#include "terraingen/types/math/Vec2.h"

#include <algorithm>

MeshData MeshGenerator::GenerateMesh(const HeightData& data, float heightMultiplier, const Curve& heightCurve)
{
	MeshData meshData;

	unsigned int size = data.size();

	VertexData vertices(size * size);
	IndexData indices((size - 1) * (size - 1) * 6);

	float topLeftX = (size - 1) / -2.0f;
	float topLeftZ = (size - 1) / 2.0f;

	int vertexIndex = 0;
	int indicesIndex = 0;
	for (unsigned int y = 0; y < size; y++)
	{
		for (unsigned int x = 0; x < size; x++)
		{
			vertices[vertexIndex].pos = Vec3(topLeftX + x, heightCurve.Evaluate(data[y][x]) * heightMultiplier, topLeftZ - y);
			vertices[vertexIndex].uv = Vec2(x / (float)(size - 1), y / (float)(size - 1));

			if (x < size - 1 && y < size - 1)
			{
				unsigned int topLeft = y * size + x;
				unsigned int topRight = y * size + (x + 1);
				unsigned int bottomLeft = (y + 1) * size + x;
				unsigned int bottomRight = (y + 1) * size + (x + 1);

				// First Tri
				indices[indicesIndex] = topLeft;
				indices[indicesIndex + 1] = bottomLeft;
				indices[indicesIndex + 2] = topRight;

				// Second Tri
				indices[indicesIndex + 3] = topRight;
				indices[indicesIndex + 4] = bottomLeft;
				indices[indicesIndex + 5] = bottomRight;

				indicesIndex += 6;
			}
			vertexIndex++;
		}
	}

	ComputeNormal(vertices, indices, size);

	meshData.vertices = vertices;
	meshData.indices = indices;

	return meshData;
}

void MeshGenerator::ComputeNormal(VertexData& vertices, const IndexData indices, int size)
{
	for (int i = 0; i < indices.size() / 3; i++)
	{
		int index = i * 3;
		unsigned int index0 = indices[index];
		unsigned int index1 = indices[index + 1];
		unsigned int index2 = indices[index + 2];

		const Vec3& pos0 = vertices[index0].pos;
		const Vec3& pos1 = vertices[index1].pos;
		const Vec3& pos2 = vertices[index2].pos;

		Vec3 edge1 = pos1 - pos0;
		Vec3 edge2 = pos2 - pos0;

		Vec3 faceNormal;
		faceNormal = Vec3::Cross(edge1, edge2);
		faceNormal.Normalise();

		vertices[index0].norm += faceNormal;
		vertices[index1].norm += faceNormal;
		vertices[index2].norm += faceNormal;
	}

	for (auto& vert : vertices)
	{
		vert.norm.Normalise();
	}
}
