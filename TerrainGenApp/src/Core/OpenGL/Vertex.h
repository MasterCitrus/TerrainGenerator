#pragma once

#include <types/math/Vec3.h>
#include <types/math/Vec2.h>

using namespace Math;

struct Vertex
{
	Vec3 position;
	Vec3 normal;
	Vec2 uv;
};