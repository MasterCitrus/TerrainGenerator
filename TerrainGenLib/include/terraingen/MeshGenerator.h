#pragma once

#include "types/Defines.h"
#include "types/math/Curve.h"
#include "types/MeshData.h"

class MeshGenerator
{
public:

	MeshData GenerateMesh(const HeightData& data, float heightMultiplier, const Curve& heightCurve);

private:
	void ComputeNormal(VertexData& vertices, const IndexData indices, int size);
};