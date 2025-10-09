#pragma once

#include "math/Vec2.h";
#include "math/Vec3.h";

#include <vector>

using namespace Math;

struct MeshData
{
	std::vector<Vec3> positions;
	std::vector<Vec3> normals;
	std::vector<Vec2> uvs;
	std::vector<unsigned int> indices;
};