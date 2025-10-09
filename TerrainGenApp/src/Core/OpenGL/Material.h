#pragma once

class Shader;
class Texture;

class Material
{
public:
	Material(Shader* shader);

	void SetTexture(Texture* texture);

private:
	Shader* shader = nullptr;
	Texture* texture = nullptr;
};