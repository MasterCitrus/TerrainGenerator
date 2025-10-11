#include "Shader.h"

#include <types/math/Vec2.h>
#include <types/math/Vec3.h>
#include <types/math/Vec4.h>
#include <types/math/Mat2.h>
#include <types/math/Mat3.h>
#include <types/math/Mat4.h>
#include <glad/glad.h>

#include <fstream>
#include <iostream>
#include <sstream>

using namespace Math;

bool Shader::Load(const std::string& vertPath, const std::string& fragPath)
{
	if (vertPath.empty() || fragPath.empty())
	{
		return false;
	}

	std::ifstream vertex(vertPath);
	std::ifstream fragment(fragPath);

	std::stringstream vStream, fStream;

	vStream << vertex.rdbuf();
	fStream << fragment.rdbuf();

	vertex.close();
	fragment.close();

	std::string vertSrc = vStream.str();
	std::string fragSrc = fStream.str();

	if (vertSrc.empty() || fragSrc.empty())
	{
		return false;
	}

	const char* vCode = vertSrc.c_str();
	const char* fCode = fragSrc.c_str();

	unsigned int vert, frag;
	int success;
	char infoLog[512];

	vert = glCreateShader(GL_VERTEX_SHADER);
	glShaderSource(vert, 1, &vCode, nullptr);
	glCompileShader(vert);
	glGetShaderiv(vert, GL_COMPILE_STATUS, &success);

	if (!success)
	{
		glGetShaderInfoLog(vert, 512, nullptr, infoLog);
		std::cout << "Vertex Shader compiled failed\n" << infoLog << std::endl;
		return false;
	}

	frag = glCreateShader(GL_FRAGMENT_SHADER);
	glShaderSource(frag, 1, &fCode, nullptr);
	glCompileShader(frag);
	glGetShaderiv(frag, GL_COMPILE_STATUS, &success);

	if (!success)
	{
		glGetShaderInfoLog(frag, 512, nullptr, infoLog);
		std::cout << "Vertex Shader compiled failed\n" << infoLog << std::endl;
		return false;
	}

	shaderID = glCreateProgram();
	glAttachShader(shaderID, vert);
	glAttachShader(shaderID, frag);
	glLinkProgram(shaderID);

	glGetProgramiv(shaderID, GL_LINK_STATUS, &success);

	if (!success)
	{
		glGetProgramInfoLog(shaderID, 512, nullptr, infoLog);
		std::cout << "Shader program failed to link\n" << infoLog << std::endl;
		return false;
	}

	glDeleteShader(vert);
	glDeleteShader(frag);

	return true;
}

bool Shader::Create(const std::string& vertSrc, const std::string& fragSrc)
{
	const char* vCode = vertSrc.c_str();
	const char* fCode = fragSrc.c_str();

	unsigned int vert, frag;
	int success;
	char infoLog[512];

	vert = glCreateShader(GL_VERTEX_SHADER);
	glShaderSource(vert, 1, &vCode, nullptr);
	glCompileShader(vert);
	glGetShaderiv(vert, GL_COMPILE_STATUS, &success);

	if (!success)
	{
		glGetShaderInfoLog(vert, 512, nullptr, infoLog);
		std::cout << "Vertex Shader compiled failed\n" << infoLog << std::endl;
		return false;
	}

	frag = glCreateShader(GL_FRAGMENT_SHADER);
	glShaderSource(frag, 1, &fCode, nullptr);
	glCompileShader(frag);
	glGetShaderiv(frag, GL_COMPILE_STATUS, &success);

	if (!success)
	{
		glGetShaderInfoLog(frag, 512, nullptr, infoLog);
		std::cout << "Vertex Shader compiled failed\n" << infoLog << std::endl;
		return false;
	}

	shaderID = glCreateProgram();
	glAttachShader(shaderID, vert);
	glAttachShader(shaderID, frag);
	glLinkProgram(shaderID);

	glGetProgramiv(shaderID, GL_LINK_STATUS, &success);

	if (!success)
	{
		glGetProgramInfoLog(shaderID, 512, nullptr, infoLog);
		std::cout << "Shader program failed to link\n" << infoLog << std::endl;
		return false;
	}

	glDeleteShader(vert);
	glDeleteShader(frag);

	return true;
}

void Shader::Bind() const
{
	glUseProgram(shaderID);
}

void Shader::SetBool(const std::string& name, bool value) const
{
	glUniform1i(glGetUniformLocation(shaderID, name.c_str()), (int)value);
}

void Shader::SetInt(const std::string& name, int value) const
{
	glUniform1i(glGetUniformLocation(shaderID, name.c_str()), value);
}

void Shader::SetInt(const std::string& name, int v1, int v2) const
{
	glUniform2i(glGetUniformLocation(shaderID, name.c_str()), v1, v2);
}

void Shader::SetInt(const std::string& name, int v1, int v2, int v3) const
{
	glUniform3i(glGetUniformLocation(shaderID, name.c_str()), v1, v2, v3);
}

void Shader::SetInt(const std::string& name, int v1, int v2, int v3, int v4) const
{
	glUniform4i(glGetUniformLocation(shaderID, name.c_str()), v1, v2, v3, v4);
}

void Shader::SetFloat(const std::string& name, float value) const
{
	glUniform1f(glGetUniformLocation(shaderID, name.c_str()), value);
}

void Shader::SetFloat(const std::string& name, float v1, float v2) const
{
	glUniform2f(glGetUniformLocation(shaderID, name.c_str()), v1, v2);
}

void Shader::SetFloat(const std::string& name, float v1, float v2, float v3) const
{
	glUniform3f(glGetUniformLocation(shaderID, name.c_str()), v1, v2, v3);
}

void Shader::SetFloat(const std::string& name, float v1, float v2, float v3, float v4) const
{
	glUniform4f(glGetUniformLocation(shaderID, name.c_str()), v1, v2, v3, v4);
}

void Shader::SetVec(const std::string& name, const Vec2& value) const
{
	glUniform2fv(glGetUniformLocation(shaderID, name.c_str()), 1, &value[0]);
}

void Shader::SetVec(const std::string& name, const Vec3& value) const
{
	glUniform3fv(glGetUniformLocation(shaderID, name.c_str()), 1, &value[0]);
}

void Shader::SetVec(const std::string& name, const Vec4& value) const
{
	glUniform4fv(glGetUniformLocation(shaderID, name.c_str()), 1, &value[0]);
}

void Shader::SetMat(const std::string& name, const Mat2& value) const
{
	glUniformMatrix2fv(glGetUniformLocation(shaderID, name.c_str()), 1, GL_FALSE, &value[0][0]);
}

void Shader::SetMat(const std::string& name, const Mat3& value) const
{
	glUniformMatrix3fv(glGetUniformLocation(shaderID, name.c_str()), 1, GL_FALSE, &value[0][0]);
}

void Shader::SetMat(const std::string& name, const Mat4& value) const
{
	glUniformMatrix4fv(glGetUniformLocation(shaderID, name.c_str()), 1, GL_FALSE, &value[0][0]);
}
