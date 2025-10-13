#include "Framebuffer.h"
#include "Texture.h"

#include <glad/glad.h>

#include <iostream>

Framebuffer::Framebuffer(unsigned int width, unsigned int height)
{
	spec.width = width;
	spec.height = height;

	glGenFramebuffers(1, &framebufferID);

	colourTexture = new Texture(width, height, nullptr, TextureType::Colour, TextureFormat::RGB, TextureWrapping::None, TextureFilter::Linear);
	depthTexture = new Texture(width, height, nullptr, TextureType::Depth, TextureFormat::RGB, TextureWrapping::None, TextureFilter::Linear);
	//glGenTextures(1, &colourTextureID);
	//glBindTexture(GL_TEXTURE_2D, colourTextureID);

	//glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, width, height, 0, GL_RGBA, GL_UNSIGNED_BYTE, 0);

	//glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
	//glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);

	//glGenTextures(1, &depthTextureID);
	//glBindTexture(GL_TEXTURE_2D, depthTextureID);

	//glTexImage2D(GL_TEXTURE_2D, 0, GL_DEPTH_STENCIL, width, height, 0, GL_DEPTH_STENCIL, GL_UNSIGNED_INT_24_8, 0);

	//glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
	//glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);

	glBindFramebuffer(GL_FRAMEBUFFER, framebufferID);
	glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, GL_TEXTURE_2D, colourTexture->GetID(), 0);
	glFramebufferTexture2D(GL_FRAMEBUFFER, GL_DEPTH_STENCIL_ATTACHMENT, GL_TEXTURE_2D, depthTexture->GetID(), 0);

	glGenRenderbuffers(1, &renderbufferID);
	glBindRenderbuffer(GL_RENDERBUFFER, renderbufferID);
	glRenderbufferStorage(GL_RENDERBUFFER, GL_DEPTH24_STENCIL8, width, height);
	glBindRenderbuffer(GL_RENDERBUFFER, 0);
	glFramebufferRenderbuffer(GL_FRAMEBUFFER, GL_DEPTH_STENCIL_ATTACHMENT, GL_RENDERBUFFER, renderbufferID);

	if (glCheckFramebufferStatus(GL_FRAMEBUFFER) != GL_FRAMEBUFFER_COMPLETE)
	{
		std::cout << "Framebuffer is not complete\n";
	}

	glBindFramebuffer(GL_FRAMEBUFFER, 0);
}

Framebuffer::~Framebuffer()
{
	delete colourTexture;
	delete depthTexture;
	glDeleteFramebuffers(1, &framebufferID);
	glDeleteRenderbuffers(1, &renderbufferID);
}

void Framebuffer::Resize(unsigned int width, unsigned int height)
{
	spec.width = width;
	spec.height = height;

	delete colourTexture;
	delete depthTexture;

	colourTexture = new Texture(width, height, nullptr, TextureType::Colour, TextureFormat::RGB, TextureWrapping::None, TextureFilter::Linear);
	depthTexture = new Texture(width, height, nullptr, TextureType::Depth, TextureFormat::RGB, TextureWrapping::None, TextureFilter::Linear);

	glBindFramebuffer(GL_FRAMEBUFFER, framebufferID);

	glFramebufferTexture(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, colourTexture->GetID(), 0);
	glFramebufferTexture(GL_FRAMEBUFFER, GL_DEPTH_STENCIL_ATTACHMENT, depthTexture->GetID(), 0);

	glBindRenderbuffer(GL_RENDERBUFFER, renderbufferID);
	glRenderbufferStorage(GL_RENDERBUFFER, GL_DEPTH24_STENCIL8, spec.width, spec.height);

	glBindRenderbuffer(GL_RENDERBUFFER, 0);
	glBindFramebuffer(GL_FRAMEBUFFER, 0);
}

void Framebuffer::Bind() const
{
	glBindFramebuffer(GL_FRAMEBUFFER, framebufferID);
	glViewport(0, 0, spec.width, spec.height);
}

void Framebuffer::Unbind() const
{
	glBindFramebuffer(GL_FRAMEBUFFER, 0);
}
