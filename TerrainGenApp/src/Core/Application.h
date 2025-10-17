#pragma once

#include "ImGui/CurveEditor.h"
#include "Events/EventBus.h"
#include "GeneratorData.h"
#include "OpenGL/Camera.h"
#include "OpenGL/Shader.h"
#include "Serializer.h"

#include <terraingen/TerrainGenerator.h>
#include <terraingen/TerrainExporter.h>
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

	static Application* Get() { return app; }

	void ExportFile(void* data);
	void SaveFile(void* data) const;
	void LoadFile(void* data);

private:
	void ProcessSDLEvents();
	void RegisterListeners();
	
	void GenerateTerrain();

	void Reset();
	void Open();
	void Save();
	void Export(FileType type);

	void OnMouseDown(MouseButtonDownEvent& event);
	void OnMouseUp(MouseButtonUpEvent& event);
	void OnMouseScroll(MouseScrollEvent& event);
	void OnMouseMove(MouseMoveEvent& event);
	void OnKeyDown(KeyDownEvent& event);
	void OnKeyUp(KeyUpEvent& event);

public:
	static Application* app;

private:
	TerrainExporter exporter;
	TerrainGenerator generator;
	EventBus cameraEvents;
	EventBus appEvents;
	GeneratorData data;
	GeneratorData dataLastFrame;
	std::vector<TerrainType> regions;
	std::filesystem::path rootDir;
	CurveEditor curveEditor;
	SDL_Window* window;
	OpenGL::Camera* camera;
	OpenGL::Material* mat;
	OpenGL::Mesh* mesh;
	OpenGL::Texture* heightMap;
	OpenGL::Texture* noiseMap;
	OpenGL::Texture* falloffMap;
	OpenGL::Texture* colourMap;
	OpenGL::Framebuffer* framebuffer;
	SDL_GLContext context;
	unsigned int fps = 0;
	OpenGL::Shader shader;
	DisplayType displayType = DisplayType::Quad;
	DisplayType previousDisplayType = displayType;
	DisplayTextureType textureDisplayType = DisplayTextureType::HeightMap;
	NoiseType noiseType = NoiseType::Perlin;
	FileType currentType;
	bool running = false;
	bool showDemoWindow = false;
	bool autoUpdateGenerator = false;
	bool viewportHovered = false;
	bool viewportFocused = false;
	bool pixelate = false;
	bool useFalloffMap = false;
	bool showExtraTextures = false;
};

static void ExportModelCallback(void* userdata, const char* const* filelist, int filter_index);
static void SaveFileCallback(void* userdata, const char* const* filelist, int filter_index);
static void SDLCALL OpenFileCallback(void* userdata, const char* const* filelist, int filter_index);