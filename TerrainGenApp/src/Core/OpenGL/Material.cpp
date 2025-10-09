#include "Material.h"

Material::Material(Shader* shader)
	: shader(shader)
{
}

void Material::SetTexture(Texture* texture)
{
	this->texture = texture;
}
