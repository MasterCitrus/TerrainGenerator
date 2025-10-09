#include "Application.h"

#include <glad/gl.h>
#include <SDL3/SDL.h>
#include <imgui.h>
#include <backends/imgui_impl_sdl3.h>
#include <backends/imgui_impl_opengl3.h>

#include <chrono>

bool Application::Initialise()
{
    if (!SDL_Init(SDL_INIT_VIDEO) != 0)
    {

        return false;
    }

    window = SDL_CreateWindow("Terrain Generator", 1280, 720, SDL_WINDOW_OPENGL | SDL_WINDOW_RESIZABLE);

    if (!window)
    {
        return false;
    }

    context = SDL_GL_CreateContext(window);
    SDL_GL_MakeCurrent(window, context);
    SDL_GL_SetSwapInterval(1);

    if (!gladLoadGL((SDL_GL_GetProcAddress)))
    {
        return false;
    }
    
    IMGUI_CHECKVERSION();
    ImGui::CreateContext();
    ImGuiIO& io = ImGui::GetIO(); (void)io;
    ImGui::StyleColorsDark();
    ImGui_ImplSDL3_InitForOpenGL(window, context);
    ImGui_ImplOpenGL3_Init("#version 460");

    running = true;

    return true;
}

void Application::Deinitialise()
{
    ImGui_ImplOpenGL3_Shutdown();
    ImGui_ImplSDL3_Shutdown();
    ImGui::DestroyContext();

    SDL_GL_DestroyContext(context);
    SDL_DestroyWindow(window);
    SDL_Quit();
}

void Application::Run()
{
    if (!Initialise()) return;

    std::chrono::time_point<std::chrono::high_resolution_clock> prevTime = std::chrono::high_resolution_clock::now();
    std::chrono::time_point<std::chrono::high_resolution_clock> currTime;

    double deltaTime = 0;
    unsigned int frames = 0;
    double fpsInterval = 0;

    SDL_Event event;

    while (running)
    {
        currTime = std::chrono::high_resolution_clock::now();
        auto duration = std::chrono::duration_cast<std::chrono::milliseconds>(currTime - prevTime);
        prevTime = currTime;

        deltaTime = duration.count() / 1000.0f;

        frames++;
        fpsInterval += deltaTime;
        if (fpsInterval >= 1.0f)
        {
            fps = frames;
            frames = 0;
            fpsInterval = 0;
        }

        while (SDL_PollEvent(&event))
        {
            ImGui_ImplSDL3_ProcessEvent(&event);
            if (event.type == SDL_EVENT_QUIT)
            {
                running = false;
            }
        }
        
        ImGui_ImplOpenGL3_NewFrame();
        ImGui_ImplSDL3_NewFrame();
        ImGui::NewFrame();

        // Update + Rendering Loop

        //

        ImGui::Render();

        ImGui_ImplOpenGL3_RenderDrawData(ImGui::GetDrawData());
        SDL_GL_SwapWindow(window);
    }

    Deinitialise();
}

void Application::Update(float delta)
{

}

void Application::Render()
{

}