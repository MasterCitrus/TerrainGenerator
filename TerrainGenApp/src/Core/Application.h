#pragma once

#include "Events/EventBus.h"
#include "GeneratorData.h"
#include "OpenGL/Camera.h"
#include "OpenGL/Shader.h"

#include <terraingen/TerrainGenerator.h>
#include <terraingen/types/TerrainType.h>

#include <filesystem>
#include <random>
#include <vector>

namespace OpenGL
{
	class Framebuffer;
	class Material;
	class Mesh;
	class Texture;
}

struct SDL_Window;
struct SDL_GLContextState;

typedef SDL_GLContextState* SDL_GLContext;


class Application
{
public:
	Application() = default;

	bool Initialise();
	void Deinitialise();

	void Run();

	void Update(float delta);

	void Render();

	void OnEvent(Event& event);

private:
	void ProcessSDLEvents();
	void RegisterListeners();
	
	void GenerateTerrain();

	void Reset();
	void Open();
	void Save();
	void Export();

	void OnMouseDown(MouseButtonDownEvent& event);
	void OnMouseUp(MouseButtonUpEvent& event);
	void OnMouseScroll(MouseScrollEvent& event);
	void OnMouseMove(MouseMoveEvent& event);
	void OnKeyDown(KeyDownEvent& event);
	void OnKeyUp(KeyUpEvent& event);

private:
	EventBus cameraEvents;
	EventBus appEvents;
	std::vector<TerrainType> regions;
	std::filesystem::path rootDir;
	GeneratorData data;
	GeneratorData dataLastFrame;
	SDL_Window* window;
	OpenGL::Camera* camera;
	OpenGL::Material* mat;
	OpenGL::Mesh* mesh;
	OpenGL::Texture* heightMap;
	OpenGL::Texture* colourMap;
	OpenGL::Framebuffer* framebuffer;
	SDL_GLContext context;
	unsigned int fps = 0;
	OpenGL::Shader shader;
	DisplayType displayType = DisplayType::Quad;
	DisplayTextureType textureDisplayType = DisplayTextureType::HeightMap;
	NoiseType noiseType = NoiseType::Perlin;
	bool running = false;
	bool showDemoWindow = false;
	bool autoUpdateGenerator = false;
	bool viewportHovered = false;
	bool viewportFocused = false;
	bool pixelate = false;
};