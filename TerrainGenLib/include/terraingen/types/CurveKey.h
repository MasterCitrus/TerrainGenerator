#pragma once

struct CurveKey
{
	float time;
	float value;
	float inTangent;
	float outTangent;
	bool selected;

	bool operator<(const CurveKey& other) const
	{
		return this->time < other.time;
	}
};