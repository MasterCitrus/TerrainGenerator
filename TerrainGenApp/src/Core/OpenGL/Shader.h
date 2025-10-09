#pragma once

namespace Math
{
	struct Vec2;
	struct Vec3;
	struct Vec4;
	struct Mat2;
	struct Mat3;
	struct Mat4;
}

#include <string>

using namespace Math;

class Shader
{
public:
	Shader() = default;

	bool Load(const std::string& vertPath, const std::string& fragPath);
	bool Create(const std::string& vertSrc, const std::string& fragSrc);

	void Bind() const;

	unsigned int GetID() const { return shaderID; }

	void SetBool(const std::string& name, bool value) const;
	void SetInt(const std::string& name, int value) const;
	void SetInt(const std::string& name, int v1, int v2) const;
	void SetInt(const std::string& name, int v1, int v2, int v3) const;
	void SetInt(const std::string& name, int v1, int v2, int v3, int v4) const;
	void SetFloat(const std::string& name, float value) const;
	void SetFloat(const std::string& name, float v1, float v2) const;
	void SetFloat(const std::string& name, float v1, float v2, float v3) const;
	void SetFloat(const std::string& name, float v1, float v2, float v3, float v4) const;
	void SetVec(const std::string& name, const Vec2& value) const;
	void SetVec(const std::string& name, const Vec3& value) const;
	void SetVec(const std::string& name, const Vec4& value) const;
	void SetMat(const std::string& name, const Mat2& value) const;
	void SetMat(const std::string& name, const Mat3& value) const;
	void SetMat(const std::string& name, const Mat4& value) const;

private:
	unsigned int shaderID = 0;
};