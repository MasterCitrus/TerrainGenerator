#pragma once

#include "types/Defines.h"
#include "types/math/Curve.h"
#include "types/MeshData.h"

class MeshGenerator
{
public:

	MeshData GenerateMesh(const HeightData& data, float heightMultiplier, const Curve& heightCurve);

private:
	Math::Vec3 ComputeNormal(int x, int y, int size, const HeightData& data);
	float Sample(int x, int y, int size, const HeightData& data) const;
};