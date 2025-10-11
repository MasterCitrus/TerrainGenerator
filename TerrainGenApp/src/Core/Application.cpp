#include "Application.h"

#include "OpenGL/Framebuffer.h"
#include "OpenGL/Texture.h"
#include "OpenGL/Shader.h"

#include <glad/glad.h>
#include <SDL3/SDL.h>
#include <SDL3/SDL_opengl.h>
#include <imgui.h>
#include <imgui_internal.h>
#include <backends/imgui_impl_sdl3.h>
#include <backends/imgui_impl_opengl3.h>

#include <chrono>
#include "Camera.h"

bool Application::Initialise()
{
    if (!SDL_Init(SDL_INIT_VIDEO))
    {
        return false;
    }

    SDL_GL_SetAttribute(SDL_GL_CONTEXT_MAJOR_VERSION, 4);
    SDL_GL_SetAttribute(SDL_GL_CONTEXT_MINOR_VERSION, 6);
    SDL_GL_SetAttribute(SDL_GL_CONTEXT_PROFILE_MASK, SDL_GL_CONTEXT_PROFILE_CORE);
    SDL_GL_SetAttribute(SDL_GL_DOUBLEBUFFER, 1);

    window = SDL_CreateWindow("Terrain Generator", 1280, 720, SDL_WINDOW_OPENGL | SDL_WINDOW_RESIZABLE);

    if (!window)
    {
        SDL_Quit();
        return false;
    }

    context = SDL_GL_CreateContext(window);
    if (!context)
    {
        SDL_DestroyWindow(window);
        SDL_Quit();
    }

    SDL_GL_MakeCurrent(window, context);
    SDL_GL_SetSwapInterval(1);

    if (!gladLoadGL())
    {
        SDL_GL_DestroyContext(context);
        SDL_DestroyWindow(window);
        SDL_Quit();
        return false;
    }
    
    IMGUI_CHECKVERSION();
    ImGui::CreateContext();
    ImGuiIO& io = ImGui::GetIO(); (void)io;
    io.ConfigFlags |= ImGuiConfigFlags_DockingEnable;
    ImGui::StyleColorsDark();
    ImGui_ImplSDL3_InitForOpenGL(window, context);
    ImGui_ImplOpenGL3_Init("#version 460");

    framebuffer = new Framebuffer(1280, 720);

    if (!framebuffer)
    {
        return false;
    }

    running = true;

    return true;
}

void Application::Deinitialise()
{
    delete framebuffer;

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
        Update(deltaTime);
        //

        ImGui::Render();

        ImGui_ImplOpenGL3_RenderDrawData(ImGui::GetDrawData());
        SDL_GL_SwapWindow(window);
    }

    Deinitialise();
}

void Application::Update(float delta)
{
    ImGuiDockNodeFlags dockspaceFlags = ImGuiDockNodeFlags_PassthruCentralNode | ImGuiDockNodeFlags_NoTabBar;

    ImGuiID dockspaceID = ImGui::DockSpaceOverViewport(0, ImGui::GetMainViewport(), dockspaceFlags);

    static bool firstTime = true;
    if (firstTime)
    {
        firstTime = false;

        ImGui::DockBuilderRemoveNode(dockspaceID);
        ImGui::DockBuilderAddNode(dockspaceID, dockspaceFlags | ImGuiDockNodeFlags_DockSpace);
        ImGui::DockBuilderSetNodeSize(dockspaceID, ImGui::GetWindowSize());

        ImGuiID mainDockspaceID = dockspaceID;
        ImGuiID right = ImGui::DockBuilderSplitNode(mainDockspaceID, ImGuiDir_Right, 0.80f, nullptr, &mainDockspaceID);

        ImGui::DockBuilderDockWindow("Viewport", mainDockspaceID);
        ImGui::DockBuilderDockWindow("GenPanel", right);
        ImGui::DockBuilderFinish(dockspaceID);
    }

    glClearColor(1.0f, 0.2f, 0.2f, 1.0f);
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

    framebuffer->Bind();

    glEnable(GL_DEPTH_TEST);

    glClearColor(1.0f, 0.2f, 0.2f, 1.0f);
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

    framebuffer->Unbind();

    if(showDemoWindow)
    {
        ImGui::ShowDemoWindow();
    }

    if (ImGui::BeginMainMenuBar())
    {
        if (ImGui::BeginMenu("Options"))
        {
            ImGui::Checkbox("##showdemowindow", &showDemoWindow);
            ImGui::SameLine();
            ImGui::Text("Show Demo Window");
            ImGui::EndMenu();
        }
        ImGui::EndMainMenuBar();
    }

    // Viewport Start    

    ImGui::Begin("Viewport");
    ImVec2 viewport = ImGui::GetContentRegionAvail();

    if (FBSpec spec = framebuffer->GetSpec(); viewport.x > 0.0f && viewport.y > 0.0f && ((float)spec.width != viewport.x || (float)spec.height != viewport.y))
    {
        framebuffer->Resize((unsigned int)viewport.x, (unsigned int)viewport.y);
    }


    ImGui::Image((void*)framebuffer->GetColourTexture()->GetID(), viewport, ImVec2(0, 1), ImVec2(1, 0));
    ImGui::End();

 

    //Viewport End

    // Generator Panel Start

    ImGui::Begin("GenPanel");
    ImGui::SeparatorText("Generator Panel");
    ImGui::End();

    // Generator Panel End
}


