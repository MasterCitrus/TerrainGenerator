#pragma once

class Framebuffer;

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
	SDL_Window* window;
	Framebuffer* framebuffer;
	SDL_GLContext context;
	unsigned int fps = 0;
	bool running = false;
	bool showDemoWindow = false;
};