#pragma once

#include "Events/EventBus.h"
#include "GeneratorData.h"
#include "OpenGL/Camera.h"
#include "OpenGL/Shader.h"

#include <random>

class Framebuffer;
class Mesh;

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

private:
	void ProcessSDLEvents();
	void RegisterListeners();

private:
	EventBus bus;
	GeneratorData data;
	SDL_Window* window;
	Camera* camera;
	Mesh* mesh;
	Framebuffer* framebuffer;
	SDL_GLContext context;
	unsigned int fps = 0;
	Shader shader;
	bool running = false;
	bool showDemoWindow = false;
};