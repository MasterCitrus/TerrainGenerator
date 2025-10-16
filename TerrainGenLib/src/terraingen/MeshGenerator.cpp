#include "terraingen/MeshGenerator.h"
#include "terraingen/types/math/Vec2.h"

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
	for (int y = 0; y < size; y++)
	{
		for (int x = 0; x < size; x++)
		{
			vertices[vertexIndex].pos = Vec3(topLeftX + x, heightCurve.Evaluate(data[y][x]) * heightMultiplier, topLeftZ - y);
			//vertices[vertexIndex].pos = ComputeNormal(x, y, size, data);
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

Math::Vec3 MeshGenerator::ComputeNormal(int x, int y, int size, const HeightData& data)
{
	float hL = Sample(x, y, size, data);
	float hR = Sample(x, y, size, data);
	float hD = Sample(x, y, size, data);
	float hU = Sample(x, y, size, data);

	Math::Vec3 normal;
	normal.x = hL - hR;
	normal.y = 2.0f;
	normal.z = hD - hU;

	normal.Normalise();

	return normal;
}

float MeshGenerator::Sample(int x, int y, int size, const HeightData& data) const
{
	if (x < 0) x = 0;
	else if (x >= size) x = size - 1;
	if (y < 0) y = 0;
	else if (y >= size) x = size - 1;

	return data[y][x];
}
