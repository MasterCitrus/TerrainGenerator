#include "Application.h"

#include "OpenGL/Framebuffer.h"
#include "OpenGL/Material.h"
#include "OpenGL/Mesh.h"
#include "OpenGL/Shader.h"
#include "OpenGL/Texture.h"

#include <MapGenerator.h>

#include <glad/glad.h>
#include <SDL3/SDL.h>
#include <SDL3/SDL_opengl.h>
#include <SDL3/SDL_events.h>
#include <imgui.h>
#include <imgui_internal.h>
#include <backends/imgui_impl_sdl3.h>
#include <backends/imgui_impl_opengl3.h>

#include <chrono>
#include <cmath>
#include <filesystem>
#include <numbers>

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

    auto path = std::filesystem::current_path();

    camera = new Camera(45.0f, 1.778f, 0.1f, 1000.f);

    meshTexture = new Texture;

    if (!shader.Load(path.string() + "\\shaders\\mesh.vert", path.string() + "\\shaders\\mesh.frag"))
    {
        return false;
    }

    running = true;

    return true;
}

void Application::Deinitialise()
{
    delete camera;
    delete framebuffer;
    delete meshTexture;

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

    Material mat(&shader);

    mat.SetTexture(meshTexture);

    mesh = new Mesh(&mat, MeshShape::Quad);

    RegisterListeners();

    GenerateTerrain();

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

        ProcessSDLEvents();
        
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

    delete mesh;

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

    glClearColor(0.2f, 0.2f, 0.2f, 1.0f);
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

    framebuffer->Bind();

    glEnable(GL_DEPTH_TEST);

    glClearColor(0.2f, 0.2f, 0.2f, 1.0f);
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

    shader.Bind();

    shader.SetVec("viewPos", camera->GetPosition());
    shader.SetMat("projection", camera->GetProjectionMatrix());
    shader.SetMat("view", camera->GetViewMatrix());

    glm::mat4 model = glm::mat4(1.0f);
    model = glm::translate(model, glm::vec3(0.0f, 0.0f, 0.0f));
    model = glm::scale(model, glm::vec3(5.0f));

    shader.SetMat("model", model);

    /////////////////////////////////////////////////////////////////////
    /////////////////////////////////////////////////////////////////////
    
    //std::vector<float> vertices = {
    //    -0.5f, -0.5f, -0.5f,  0.0f, 0.0f,
    //     0.5f, -0.5f, -0.5f,  1.0f, 0.0f,
    //     0.5f,  0.5f, -0.5f,  1.0f, 1.0f,
    //     0.5f,  0.5f, -0.5f,  1.0f, 1.0f,
    //    -0.5f,  0.5f, -0.5f,  0.0f, 1.0f,
    //    -0.5f, -0.5f, -0.5f,  0.0f, 0.0f,

    //    -0.5f, -0.5f,  0.5f,  0.0f, 0.0f,
    //     0.5f, -0.5f,  0.5f,  1.0f, 0.0f,
    //     0.5f,  0.5f,  0.5f,  1.0f, 1.0f,
    //     0.5f,  0.5f,  0.5f,  1.0f, 1.0f,
    //    -0.5f,  0.5f,  0.5f,  0.0f, 1.0f,
    //    -0.5f, -0.5f,  0.5f,  0.0f, 0.0f,

    //    -0.5f,  0.5f,  0.5f,  1.0f, 0.0f,
    //    -0.5f,  0.5f, -0.5f,  1.0f, 1.0f,
    //    -0.5f, -0.5f, -0.5f,  0.0f, 1.0f,
    //    -0.5f, -0.5f, -0.5f,  0.0f, 1.0f,
    //    -0.5f, -0.5f,  0.5f,  0.0f, 0.0f,
    //    -0.5f,  0.5f,  0.5f,  1.0f, 0.0f,

    //     0.5f,  0.5f,  0.5f,  1.0f, 0.0f,
    //     0.5f,  0.5f, -0.5f,  1.0f, 1.0f,
    //     0.5f, -0.5f, -0.5f,  0.0f, 1.0f,
    //     0.5f, -0.5f, -0.5f,  0.0f, 1.0f,
    //     0.5f, -0.5f,  0.5f,  0.0f, 0.0f,
    //     0.5f,  0.5f,  0.5f,  1.0f, 0.0f,

    //    -0.5f, -0.5f, -0.5f,  0.0f, 1.0f,
    //     0.5f, -0.5f, -0.5f,  1.0f, 1.0f,
    //     0.5f, -0.5f,  0.5f,  1.0f, 0.0f,
    //     0.5f, -0.5f,  0.5f,  1.0f, 0.0f,
    //    -0.5f, -0.5f,  0.5f,  0.0f, 0.0f,
    //    -0.5f, -0.5f, -0.5f,  0.0f, 1.0f,

    //    -0.5f,  0.5f, -0.5f,  0.0f, 1.0f,
    //     0.5f,  0.5f, -0.5f,  1.0f, 1.0f,
    //     0.5f,  0.5f,  0.5f,  1.0f, 0.0f,
    //     0.5f,  0.5f,  0.5f,  1.0f, 0.0f,
    //    -0.5f,  0.5f,  0.5f,  0.0f, 0.0f,
    //    -0.5f,  0.5f, -0.5f,  0.0f, 1.0f
    //};

    //unsigned int VBO, VAO;
    //glGenVertexArrays(1, &VAO);
    //glGenBuffers(1, &VBO);

    //glBindVertexArray(VAO);

    //glBindBuffer(GL_ARRAY_BUFFER, VBO);
    //glBufferData(GL_ARRAY_BUFFER, sizeof(vertices), vertices.data(), GL_STATIC_DRAW);

    //// position attribute
    //glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 5 * sizeof(float), (void*)0);
    //glEnableVertexAttribArray(0);
    //// texture coord attribute
    //glVertexAttribPointer(1, 2, GL_FLOAT, GL_FALSE, 5 * sizeof(float), (void*)(3 * sizeof(float)));
    //glEnableVertexAttribArray(1);

    //glDrawArrays(GL_TRIANGLES, 0, 36);
    
    /////////////////////////////////////////////////////////////////////
    /////////////////////////////////////////////////////////////////////

    mesh->Draw();

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
        camera->SetViewportSize(viewport.x, viewport.y);
    }


    ImGui::Image((void*)framebuffer->GetColourTexture()->GetID(), viewport, ImVec2(0, 1), ImVec2(1, 0));
    ImGui::End();

 

    //Viewport End

    // Generator Panel Start

    ImGui::Begin("GenPanel");
    ImVec2 panelSize = ImGui::GetContentRegionAvail();
    ImGui::SeparatorText("Generator Panel");
    ///////////////////////////////////////
    ////// Seed ///////////////////////////
    ///////////////////////////////////////
    ImGui::Text("Seed");
    ImGui::SameLine();
    ImGui::InputScalar("##seedinput", ImGuiDataType_U32, &data.seed);
    ImGui::SameLine();
    if (ImGui::Button("Random"))
    {
        static std::random_device rd;
        static std::mt19937 mt(rd());
        std::uniform_int_distribution<unsigned int> distrib(0, std::numeric_limits<unsigned int>::max());

        data.seed = distrib(mt);
    }
    ///////////////////////////////////////
    ////// Noise Scale ////////////////////
    ///////////////////////////////////////
    ImGui::Text("Noise Scale");
    ImGui::SameLine();
    ImGui::DragFloat("##noisescale", &data.noiseScale, 0.01f, 0.00001f, 100.0f, "%.5f");
    ///////////////////////////////////////
    ////// Octaves ////////////////////////
    ///////////////////////////////////////
    ImGui::Text("Octaves");
    ImGui::SameLine();
    unsigned int minValue = 1;
    ImGui::DragScalar("##octaves", ImGuiDataType_U32, &data.octaves, 1.0f, &minValue);
    ///////////////////////////////////////
    ////// Persistence ///////////////////
    ///////////////////////////////////////
    ImGui::Text("Persistence");
    ImGui::SameLine();
    ImGui::SliderFloat("##persistence", &data.persistence, 0.0f, 1.0f);
    ///////////////////////////////////////
    ////// Lacunarity /////////////////////
    ///////////////////////////////////////
    ImGui::Text("Lacunarity");
    ImGui::SameLine();
    ImGui::DragFloat("##lacunarity", &data.lacunarity, 0.01f, 0.0f, 100.0f);
    ///////////////////////////////////////
    ////// Offset /////////////////////////
    ///////////////////////////////////////
    ImGui::Text("Offset");
    ImGui::SameLine();
    ImGui::DragFloat2("##offset", &data.offset[0], 0.01f);
    ///////////////////////////////////////
    ////// Auto Update ////////////////////
    ///////////////////////////////////////
    ImGui::Text("Auto Update");
    ImGui::SameLine();
    ImGui::Checkbox("##autoupdate", &autoUpdateGenerator);
    ///////////////////////////////////////
    ////// Generate Button ////////////////
    ///////////////////////////////////////
    if (ImGui::Button("Generate", ImVec2(panelSize.x, ImGui::CalcTextSize("Generate").y + ImGui::GetStyle().FramePadding.x * 2.0f)))
    {
        GenerateTerrain();
    }
    ImGui::End();

    // Generator Panel End

    if (autoUpdateGenerator)
    {
        if (data != dataLastFrame)
        {
            GenerateTerrain();
            dataLastFrame = data;
        }
    }
}


void Application::ProcessSDLEvents()
{
    SDL_Event event;

    while (SDL_PollEvent(&event))
    {
        ImGui_ImplSDL3_ProcessEvent(&event);

        switch (event.type)
        {
            case SDL_EVENT_QUIT:
                running = false;
                break;
            case SDL_EVENT_KEY_DOWN:
            {
                KeyDownEvent e(event.key.scancode, event.key.repeat);
                bus.Dispatch(e);
                break;
            }
            case SDL_EVENT_KEY_UP:
            {
                KeyUpEvent e(event.key.scancode, false);
                bus.Dispatch(e);
                break;
            }
            case SDL_EVENT_MOUSE_MOTION:
            {
                MouseMoveEvent e(event.motion.x, event.motion.y, event.motion.xrel, event.motion.yrel);
                bus.Dispatch(e);
                break;
            }
            case SDL_EVENT_MOUSE_WHEEL:
            {
                MouseScrollEvent e(event.wheel.x, event.wheel.y);
                bus.Dispatch(e);
                break;
            }
            case SDL_EVENT_MOUSE_BUTTON_DOWN:
            {
                MouseButtonDownEvent e(event.button.button, event.button.x, event.button.y);
                bus.Dispatch(e);
                break;
            }
            case SDL_EVENT_MOUSE_BUTTON_UP:
            {
                MouseButtonUpEvent e(event.button.button, event.button.x, event.button.y);
                bus.Dispatch(e);
                break;
            }
            default:
                break;
        }
    }
}

void Application::RegisterListeners()
{
    bus.Subscribe(EventType::MouseButtonDown, [this](Event& event) { camera->OnEvent(event); });
    bus.Subscribe(EventType::MouseButtonUp, [this](Event& event) { camera->OnEvent(event); });
    bus.Subscribe(EventType::MouseMove, [this](Event& event) { camera->OnEvent(event); });
    bus.Subscribe(EventType::MouseScroll, [this](Event& event) { camera->OnEvent(event); });
    bus.Subscribe(EventType::KeyDown, [this](Event& event) { camera->OnEvent(event); });
    bus.Subscribe(EventType::KeyUp, [this](Event& event) { camera->OnEvent(event); });
}

void Application::GenerateTerrain()
{
    MapGenerator generator;

    auto map = generator.GenerateNoiseMap(data.noiseScale, data.octaves, data.persistence, data.lacunarity, data.offset, data.seed);

    unsigned int size = map.size();

    std::vector<unsigned char> pixels(size * size);

    int k = 0;
    for (int y = 0; y < size; y++)
    {
        for (int x = 0; x < size; x++)
        {
            pixels[k] = std::floor(map[y][x] * 255);
            k++;
        }
    }

    meshTexture->Create(size, size, pixels.data(), TextureType::Colour, TextureFormat::GS, TextureWrapping::Repeat, TextureFilter::Linear);
}
