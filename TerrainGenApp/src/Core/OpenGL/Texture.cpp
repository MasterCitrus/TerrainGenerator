#include "Texture.h"

#include <glad/gl.h>

Texture::Texture(unsigned int width, unsigned int height, TextureType type, TextureFormat format)
{
	Create(width, height, type, format);
}

Texture::Texture(const std::string& path, TextureType type)
{
	Load(path, type);
}

Texture::~Texture()
{
	if (textureID != 0)
	{
		glDeleteTextures(1, &textureID);
		GLuint
	}
}

bool Texture::Create(unsigned int width, unsigned int height, TextureType type, TextureFormat format)
{
	if (textureID != 0)
	{
		glDeleteTextures(1, &textureID);
		textureID = 0;
		width = 0;
		height = 0;
	}

	this->width = width;
	this->height = height;
	this->type = type;

	glGenTextures(1, &textureID);
	glBindTexture(GL_TEXTURE_2D, textureID);

	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);

	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);

	if(type == TextureType::Colour)
	{
		switch (format)
		{
			case TextureFormat::GS:
				glTexImage2D(GL_TEXTURE_2D, 0, GL_RED, width, height, 0, GL_RED, GL_UNSIGNED_BYTE, 0);
				break;
			case TextureFormat::GSA:
				glTexImage2D(GL_TEXTURE_2D, 0, GL_RG, width, height, 0, GL_RG, GL_UNSIGNED_BYTE, 0);
				break;
			case TextureFormat::RGB:
				glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB, width, height, 0, GL_RGB, GL_UNSIGNED_BYTE, 0);
				break;
			case TextureFormat::RGBA:
				glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, width, height, 0, GL_RGBA, GL_UNSIGNED_BYTE, 0);
				break;
			default:
				return false;
		}
	}
	else if (type == TextureType::Depth)
	{
		glTexImage2D(GL_TEXTURE_2D, 0, GL_DEPTH_COMPONENT24, width, height, 0, GL_DEPTH_COMPONENT, GL_FLOAT, 0);
	}

	glBindTexture(GL_TEXTURE_2D, 0);

	return true;
}

bool Texture::Load(const std::string& path, TextureType type)
{
	return false;
}
