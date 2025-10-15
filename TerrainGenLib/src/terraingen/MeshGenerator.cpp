#include "terraingen/MeshGenerator.h"
#include "terraingen/types/math/Vec2.h"

MeshData MeshGenerator::GenerateMesh(const HeightData& data, float heightMultiplier)
{
	MeshData meshData;

	unsigned int size = data.size();

	VertexData vertices(size * size);
	IndexData indices((size - 1) * (size - 1) * 6);

	float topLeftX = (size - 1) / -2.0f;
	float topLeftZ = (size - 1) / 2.0f;

	int vertexIndex = 0;
	int indicesIndex = 0;
	for (int y = 0; y < size; y++)
	{
		for (int x = 0; x < size; x++)
		{
			vertices[vertexIndex].pos = Vec3(topLeftX + x, data[y][x] * heightMultiplier, topLeftZ - y);
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

	meshData.vertices = vertices;
	meshData.indices = indices;

	return meshData;
}
