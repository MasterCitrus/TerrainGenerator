#pragma once

#include "math/Vec2.h"
#include "math/Vec3.h"

using namespace Math;

struct Vertex
{
	Vec3 pos = { 0.0f, 0.0f, 0.0f };
	Vec3 norm = { 0.0f, 0.0f, 0.0f };
	Vec2 uv = { 0.0f, 0.0f };
};