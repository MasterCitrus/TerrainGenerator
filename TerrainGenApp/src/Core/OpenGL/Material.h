#pragma once


namespace OpenGL
{
	class Shader;
	class Texture;

	class Material
	{
	public:
		Material(Shader* shader);

		Texture* GetTexture() const { return texture; }
		void SetTexture(Texture* texture);

		void Apply();

	private:
		Shader* shader = nullptr;
		Texture* texture = nullptr;
	};
}
