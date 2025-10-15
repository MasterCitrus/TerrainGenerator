#pragma once

#include "types/Defines.h"
#include "types/MeshData.h"

class MeshGenerator
{
public:

	MeshData GenerateMesh(const HeightData& data, float heightMultiplier);

};