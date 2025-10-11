#pragma once

class Texture;

struct FBSpec
{
	unsigned int width = 0;
	unsigned int height = 0;

	FBSpec() = default;
	FBSpec(unsigned int width, unsigned int height)
		: width(width), height(height) { }
};

class Framebuffer
{
public:
	Framebuffer(unsigned int width, unsigned int height);
	~Framebuffer();

	void Resize(unsigned int width, unsigned int height);

	void Bind() const;
	void Unbind() const;

	Texture* GetColourTexture() const { return colourTexture; }
	Texture* GetDepthTexture() const { return depthTexture; }
	unsigned int GetID() const { return framebufferID; }
	unsigned int GetColourID() const { return framebufferID; }
	unsigned int GetDepthID() const { return framebufferID; }

	FBSpec GetSpec() const { return spec; }

private:
	FBSpec spec;
	unsigned int framebufferID = 0;
	unsigned int renderbufferID = 0;
	Texture* colourTexture = nullptr;
	Texture* depthTexture = nullptr;
};