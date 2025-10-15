#include "Material.h"
#include "Shader.h"
#include "Texture.h"

using namespace OpenGL;

Material::Material(Shader* shader)
	: shader(shader)
{
}

void Material::SetTexture(Texture* texture)
{
	this->texture = texture;
}

void Material::Apply()
{
	shader->SetInt("texture1", 1);
	texture->Bind(1);
}
