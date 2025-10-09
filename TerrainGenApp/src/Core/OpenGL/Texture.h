#pragma once

#include <string>

enum class TextureType : uint8_t
{
	Colour,
	Depth
};

enum class TextureFormat : uint8_t
{
	GS,
	GSA,
	RGB,
	RGBA
};

class Texture
{
public:
	Texture(unsigned int width, unsigned int height, TextureType type, TextureFormat format);
	Texture(const std::string& path, TextureType type);
	~Texture();

	bool Create(unsigned int width, unsigned int height, TextureType type, TextureFormat format);
	bool Load(const std::string& path, TextureType type);

	unsigned int GetID() const { return textureID; }
	unsigned int GetWidth() const { return width; }
	unsigned int GetHeight() const { return height; }

private:
	unsigned int textureID = 0;
	unsigned int width;
	unsigned int height;
	TextureType type;
};