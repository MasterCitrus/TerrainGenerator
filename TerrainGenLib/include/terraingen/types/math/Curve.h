#pragma once
#include "../CurveKey.h"

#include <vector>

class Curve
{
public:
	Curve();

	float Evaluate(float time) const;

	void AddKey(const CurveKey& key);

	std::vector<CurveKey>& GetKeys() { return keys; }

private:
	std::vector<CurveKey> keys;
};