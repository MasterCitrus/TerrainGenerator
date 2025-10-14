#pragma once

#include "Events/EventBus.h"
#include "GeneratorData.h"
#include "OpenGL/Camera.h"
#include "OpenGL/Shader.h"

#include <terraingen/MapGenerator.h>
#include <terraingen/types/TerrainType.h>

#include <filesystem>
#include <random>
#include <vector>

class Framebuffer;
class Mesh;
class Texture;

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
	std::vector<TerrainType> terrainTypes;
	std::filesystem::path rootDir;
	GeneratorData data;
	GeneratorData dataLastFrame;
	SDL_Window* window;
	Camera* camera;
	Mesh* mesh;
	Texture* heightMap;
	Texture* colourMap;
	Framebuffer* framebuffer;
	SDL_GLContext context;
	unsigned int fps = 0;
	Shader shader;
	DisplayType displayType = DisplayType::HeightMap;
	NoiseType noiseType = NoiseType::Perlin;
	bool running = false;
	bool showDemoWindow = false;
	bool autoUpdateGenerator = false;
	bool viewportHovered = false;
	bool viewportFocused = false;
	bool pixelate = false;
};