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

enum class TextureWrapping : uint8_t
{
	None,
	Repeat,
	MirroredRepeat,
	ClampToEdge,
	ClampToBorder
};

enum class TextureFilter : uint8_t
{
	Nearest,
	Linear,
	NearestMipmapNearest,
	LinearMipmapNearest,
	NearestMipmapLinear,
	LinearMipmapLinear,
};

class Texture
{
public:
	Texture(unsigned int width, unsigned int height, TextureType type, TextureFormat format, TextureWrapping wrap, TextureFilter filter);
	Texture(const std::string& path, TextureType type, TextureWrapping wrap, TextureFilter filter);
	~Texture();

	bool Create(unsigned int width, unsigned int height, TextureType type, TextureFormat format, TextureWrapping wrap, TextureFilter filter);
	bool Load(const std::string& path, TextureType type, TextureWrapping wrap, TextureFilter filter);

	void Bind(int slot = 0) const;
	void Unbind() const;

	unsigned int GetID() const { return textureID; }
	unsigned int GetWidth() const { return width; }
	unsigned int GetHeight() const { return height; }

	operator unsigned int() const { return textureID; }

private:
	unsigned int textureID = 0;
	unsigned int width;
	unsigned int height;
	TextureType type;
};