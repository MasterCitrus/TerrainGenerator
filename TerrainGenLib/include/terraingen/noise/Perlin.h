#pragma once

#include <random>
#include <vector>

class Perlin
{
public:
	Perlin(unsigned int seed = std::default_random_engine::default_seed);

	float Noise(float x, float y, float z = 0.0f);

private:
	static float Fade(float t);
	static float Lerp(float t, float a, float b);
	static float Grad(int hash, float x, float y, float z);

private:
	std::vector<int> permutationArray;
};