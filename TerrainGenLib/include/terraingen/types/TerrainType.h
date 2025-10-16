#pragma once
#include <terraingen/types/math/Vec3.h>

#include <string>

struct TerrainType
{
	std::string name;
	Math::Vec3 colour;
	float height;

	bool operator<(const TerrainType& other) const
	{
		return this->height < other.height;
	}
};