#pragma once
#include "../CurveKey.h"

#include <vector>

class Curve
{
public:
	Curve();

	float Evaluate(float time) const;

	void AddKey(const CurveKey& key);

	void ClearKeys() { keys.clear(); }

	std::vector<CurveKey>& GetKeys() { return keys; }
	std::vector<CurveKey> Keys() const { return keys; }

	int AmountOfKeys() const { return keys.size(); }

private:
	std::vector<CurveKey> keys;
};