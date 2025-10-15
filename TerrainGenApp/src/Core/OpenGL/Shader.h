#pragma once

#include <glm/mat2x2.hpp>
#include <glm/mat3x3.hpp>
#include <glm/mat4x4.hpp>

#include <string>

namespace OpenGL
{
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
		void SetVec(const std::string& name, const glm::vec2& value) const;
		void SetVec(const std::string& name, const glm::vec3& value) const;
		void SetVec(const std::string& name, const glm::vec4& value) const;
		void SetMat(const std::string& name, const glm::mat2& value) const;
		void SetMat(const std::string& name, const glm::mat3& value) const;
		void SetMat(const std::string& name, const glm::mat4& value) const;

	private:
		unsigned int shaderID = 0;
	};
}
