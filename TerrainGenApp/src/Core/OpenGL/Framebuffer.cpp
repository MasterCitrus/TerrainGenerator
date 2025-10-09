#include "Framebuffer.h"
#include "Texture.h"

#include <glad/gl.h>

#include <iostream>

Framebuffer::Framebuffer(unsigned int width, unsigned int height)
{
	spec.width = width;
	spec.height = height;

	glGenFramebuffers(1, &framebufferID);

	colourTexture = new Texture(width, height, TextureType::Colour, TextureFormat::RGBA);
	depthTexture = new Texture(width, height, TextureType::Depth, TextureFormat::RGBA);

	glBindFramebuffer(GL_FRAMEBUFFER, framebufferID);
	glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, GL_TEXTURE_2D, colourTexture->GetID(), 0);
	glFramebufferTexture2D(GL_FRAMEBUFFER, GL_DEPTH_ATTACHMENT, GL_TEXTURE_2D, depthTexture->GetID(), 0);

	glGenRenderbuffers(1, &renderbufferID);
	glBindRenderbuffer(GL_RENDERBUFFER, renderbufferID);
	glRenderbufferStorage(GL_RENDERBUFFER, GL_DEPTH24_STENCIL8, width, height);

	if (glCheckFramebufferStatus(GL_FRAMEBUFFER) != GL_FRAMEBUFFER_COMPLETE)
	{
		std::cout << "Framebuffer is not complete\n";
	}

	glBindFramebuffer(GL_FRAMEBUFFER, 0);
	glBindRenderbuffer(GL_RENDERBUFFER, 0);
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

	colourTexture = new Texture(width, height, TextureType::Colour, TextureFormat::RGBA);
	depthTexture = new Texture(width, height, TextureType::Depth, TextureFormat::RGBA);

	glFramebufferTexture(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, colourTexture->GetID(), 0);
	glFramebufferTexture(GL_FRAMEBUFFER, GL_DEPTH_STENCIL_ATTACHMENT, depthTexture->GetID(), 0);

	glBindRenderbuffer(GL_RENDERBUFFER, renderbufferID);
	glRenderbufferStorage(GL_RENDERBUFFER, GL_DEPTH24_STENCIL8, spec.width, spec.height);
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
