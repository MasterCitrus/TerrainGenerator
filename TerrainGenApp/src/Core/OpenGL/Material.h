#pragma once

class Shader;
class Texture;

class Material
{
public:
	Material(Shader* shader);

	void SetTexture(Texture* texture);

	void Apply();

private:
	Shader* shader = nullptr;
	Texture* texture = nullptr;
};