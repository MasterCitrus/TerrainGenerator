#include "noise/Perlin.h"

#include <algorithm>
#include <cmath>
#include <numeric>

Perlin::Perlin(unsigned int seed)
{
	permutationArray.resize(256);

	std::iota(permutationArray.begin(), permutationArray.end(), 0);

	std::default_random_engine engine(seed);
	std::shuffle(permutationArray.begin(), permutationArray.end(), engine);

	permutationArray.insert(permutationArray.end(), permutationArray.begin(), permutationArray.end());
}

float Perlin::Noise(float x, float y, float z)
{
	int X = (int)std::floor(x) & 255;
	int Y = (int)std::floor(y) & 255;
	int Z = (int)std::floor(z) & 255;

	x -= std::floor(x);
	y -= std::floor(y);
	z -= std::floor(z);

	float u = Fade(x);
	float v = Fade(y);
	float w = Fade(z);

	int A = permutationArray[X] + Y;
	int AA = permutationArray[A] + Z;
	int AB = permutationArray[A + 1] + Z;
	int B = permutationArray[X + 1] + Y;
	int BA = permutationArray[B] + Z;
	int BB = permutationArray[B + 1] + Z;

	return Lerp(w,
		Lerp(v,
			Lerp(u, Grad(permutationArray[AA], x, y, z), Grad(permutationArray[BA], x - 1, y, z)),
			Lerp(u, Grad(permutationArray[AB], x, y - 1, z), Grad(permutationArray[BB], x - 1, y - 1, z))
		),
		Lerp(v,
			Lerp(u, Grad(permutationArray[AA + 1], x, y, z - 1), Grad(permutationArray[BA + 1], x - 1, y, z - 1)),
			Lerp(u, Grad(permutationArray[AB + 1], x, y - 1, z - 1), Grad(permutationArray[BB + 1], x - 1, y - 1, z - 1))
		)
	);
}

float Perlin::Fade(float t)
{
	return t * t * t * (t * (t * 6 - 15) + 10);
}

float Perlin::Lerp(float t, float a, float b)
{
	return a + t * (b - a);
}

float Perlin::Grad(int hash, float x, float y, float z)
{
	int h = hash & 15;
	float u = h < 8 ? x : y;
	float v = h < 4 ? y : (h == 12 || h == 14 ? x : z);
	return ((h & 1) ? -u : u) + ((h & 2) ? -v : v);
}
