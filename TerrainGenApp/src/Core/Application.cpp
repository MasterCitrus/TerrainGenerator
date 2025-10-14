#include "Application.h"

#include "OpenGL/Framebuffer.h"
#include "OpenGL/Material.h"
#include "OpenGL/Mesh.h"
#include "OpenGL/Shader.h"
#include "OpenGL/Texture.h"

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

    rootDir = std::filesystem::current_path();

    camera = new Camera(45.0f, 1.778f, 0.1f, 1000.f);

    meshTexture = new Texture;

    if (!shader.Load(rootDir.string() + "\\shaders\\mesh.vert", rootDir.string() + "\\shaders\\mesh.frag"))
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
        if (ImGui::BeginMenu("File"))
        {
            if (ImGui::MenuItem("New Terrain", "Ctrl + N"))
            {
                Reset();
            }
            if (ImGui::MenuItem("Load Terrain", "Ctrl + O"))
            {
                Open();
            }
            if (ImGui::MenuItem("Save Terrain", "Ctrl + S"))
            {
                Save();
            }
            if (ImGui::BeginMenu("Export Terrain"))
            {

                ImGui::EndMenu();
            }

            ImGui::EndMenu();
        }
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
    viewportHovered = ImGui::IsWindowHovered();
    viewportFocused = ImGui::IsWindowFocused();

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
    if(ImGui::CollapsingHeader("Settings", ImGuiTreeNodeFlags_DefaultOpen))
    {
        // Drop down table
        if (ImGui::BeginTable("Dropdowns", 2))
        {
            ImGui::TableSetupColumn("Name", ImGuiTableColumnFlags_WidthFixed, 100.0f);
            ImGui::TableSetupColumn("Dropdown", ImGuiTableColumnFlags_WidthStretch, 100.0f);

            // Display Type Select
            ImGui::TableNextRow();
            ImGui::TableSetColumnIndex(0);
            ImGui::Text("Display Type");
            ImGui::TableSetColumnIndex(1);
            ImGui::SetNextItemWidth(-1.0f);
            if (ImGui::BeginCombo("##displaytype", displayTypeNames[(uint8_t)displayType]))
            {
                for (int i = 0; i < IM_ARRAYSIZE(displayTypeNames); ++i)
                {
                    const bool isSelected = (displayType == static_cast<DisplayType>(i));
                    if (ImGui::Selectable(displayTypeNames[i], isSelected))
                    {
                        displayType = static_cast<DisplayType>(i);
                    }

                    if (isSelected)
                    {
                        ImGui::SetItemDefaultFocus();
                    }
                }
                ImGui::EndCombo();
            }
            // Noise Type Select
            ImGui::TableNextRow();
            ImGui::TableSetColumnIndex(0);
            ImGui::Text("Noise Type");
            ImGui::TableSetColumnIndex(1);
            ImGui::SetNextItemWidth(-1.0f);
            if (ImGui::BeginCombo("##noisetype", noiseTypeNames[(uint8_t)noiseType]))
            {
                for (int i = 0; i < IM_ARRAYSIZE(noiseTypeNames); ++i)
                {
                    const bool isSelected = (noiseType == static_cast<NoiseType>(i));
                    if (ImGui::Selectable(noiseTypeNames[i], isSelected))
                    {
                        noiseType = static_cast<NoiseType>(i);
                        GenerateTerrain();
                    }

                    if (isSelected)
                    {
                        ImGui::SetItemDefaultFocus();
                    }
                }
                ImGui::EndCombo();
            }

            ImGui::EndTable();
        }
        // Settings Table
        if (ImGui::BeginTable("GenSettings", 3, ImGuiTableFlags_BordersInnerV | ImGuiTableFlags_BordersOuterH))
        {
            ImGui::TableSetupColumn("Name", ImGuiTableColumnFlags_WidthFixed, 80.0f);
            ImGui::TableSetupColumn("Input", ImGuiTableColumnFlags_WidthStretch, 100.0f);
            ImGui::TableSetupColumn("Buttons", ImGuiTableColumnFlags_WidthFixed, 60.0f);
            ///////////////////////////////////////
            ////// Seed ///////////////////////////
            ///////////////////////////////////////
            ImGui::TableNextRow();
            ImGui::TableSetColumnIndex(0);
            ImGui::Text("Seed");
            ImGui::TableSetColumnIndex(1);
            ImGui::SetNextItemWidth(-1.0f);
            ImGui::InputScalar("##seedinput", ImGuiDataType_U32, &data.seed);
            ImVec2 seedDragSize = ImGui::GetItemRectSize();
            ImGui::TableSetColumnIndex(2);
            ImVec2 seedColumnWidth = ImGui::GetContentRegionAvail();
            if (ImGui::Button("Random", ImVec2(seedColumnWidth.x - 3.0f, seedDragSize.y)))
            {
                static std::random_device rd;
                static std::mt19937 mt(rd());
                std::uniform_int_distribution<unsigned int> distrib(0, std::numeric_limits<unsigned int>::max());

                data.seed = distrib(mt);
            }
            ///////////////////////////////////////
            ////// Noise Scale ////////////////////
            ///////////////////////////////////////
            ImGui::TableNextRow();
            ImGui::TableSetColumnIndex(0);
            ImGui::Text("Noise Scale");
            ImGui::TableSetColumnIndex(1);
            ImGui::SetNextItemWidth(-1.0f);
            ImGui::DragFloat("##noisescale", &data.noiseScale, 0.01f, 1.0f, 200.0f, "%.2f");
            ImGui::TableSetColumnIndex(2);
            if (ImGui::Button("Default##seed"))
            {
                data.seed = 20.0f;
            }
            ///////////////////////////////////////
            ////// Octaves ////////////////////////
            ///////////////////////////////////////
            ImGui::TableNextRow();
            ImGui::TableSetColumnIndex(0);
            ImGui::Text("Octaves");
            ImGui::TableSetColumnIndex(1);
            unsigned int minValue = 1;
            unsigned int maxValue = 15;
            ImGui::SetNextItemWidth(-1.0f);
            ImGui::DragScalar("##octaves", ImGuiDataType_U32, &data.octaves, 1.0f, &minValue, &maxValue);
            ImVec2 octaveDragSize = ImGui::GetItemRectSize();
            ImGui::TableSetColumnIndex(2);
            ImVec2 octaveColumnWidth = ImGui::GetContentRegionAvail();
            if (ImGui::Button("-", ImVec2((octaveColumnWidth.x * .5f) - 2.5f, octaveDragSize.y)))
            {
                data.octaves--;
                if (data.octaves < 1) data.octaves = 1;
            }
            ImGui::SameLine(0.0f, 2.0f);
            if (ImGui::Button("+", ImVec2((octaveColumnWidth.x * .5f) - 2.5f, octaveDragSize.y)))
            {
                data.octaves++;
                if (data.octaves > 15) data.octaves = 15;
            }
            ///////////////////////////////////////
            ////// Persistence ///////////////////
            ///////////////////////////////////////
            ImGui::TableNextRow();
            ImGui::TableSetColumnIndex(0);
            ImGui::Text("Persistence");
            ImGui::TableSetColumnIndex(1);
            ImGui::SetNextItemWidth(-1.0f);
            ImGui::SliderFloat("##persistence", &data.persistence, 0.0f, 1.0f, "%.2f");
            ImGui::TableSetColumnIndex(2);
            if (ImGui::Button("Default##persistence"))
            {
                data.persistence = 0.5f;
            }
            ///////////////////////////////////////
            ////// Lacunarity /////////////////////
            ///////////////////////////////////////
            ImGui::TableNextRow();
            ImGui::TableSetColumnIndex(0);
            ImGui::Text("Lacunarity");
            ImGui::TableSetColumnIndex(1);
            ImGui::SetNextItemWidth(-1.0f);
            ImGui::DragFloat("##lacunarity", &data.lacunarity, 0.01f, 0.0f, 100.0f, "%.2f");
            ImGui::TableSetColumnIndex(2);
            if (ImGui::Button("Default##lacunarity"))
            {
                data.lacunarity = 2.0f;
            }
            ///////////////////////////////////////
            ////// Offset /////////////////////////
            ///////////////////////////////////////
            ImGui::TableNextRow();
            ImGui::TableSetColumnIndex(0);
            ImGui::Text("Offset");
            ImGui::TableSetColumnIndex(1);
            ImGui::SetNextItemWidth(-1.0f);
            ImGui::DragFloat2("##offset", &data.offset[0], 0.01f, 0.0f, 0.0f, "%.2f");
            ImGui::TableSetColumnIndex(2);
            if (ImGui::Button("Default##offset"))
            {
                data.offset = Vec2::Zero();
            }
            ///////////////////////////////////////
            ////// Auto Update ////////////////////
            ///////////////////////////////////////
            ImGui::TableNextRow();
            ImGui::TableSetColumnIndex(0);
            ImGui::Text("Auto Update");
            ImGui::TableSetColumnIndex(1);
            ImGui::Checkbox("##autoupdate", &autoUpdateGenerator);
            ///////////////////////////////////////
            ////// Pixelate ///////////////////////
            ///////////////////////////////////////
            ImGui::TableNextRow();
            ImGui::TableSetColumnIndex(0);
            ImGui::Text("Pixelate");
            ImGui::TableSetColumnIndex(1);
            if (ImGui::Checkbox("##pixelate", &pixelate))
            {
                GenerateTerrain();
            }

            ImGui::EndTable();
        }

        ///////////////////////////////////////
        ////// Generate Button ////////////////
        ///////////////////////////////////////
        if (ImGui::Button("Generate", ImVec2(panelSize.x, ImGui::CalcTextSize("Generate").y + ImGui::GetStyle().FramePadding.x * 2.0f)))
        {
            GenerateTerrain();
        }
        // Generate Random Button
        if (ImGui::Button("Generate Random", ImVec2(panelSize.x, ImGui::CalcTextSize("Generate Random").y + ImGui::GetStyle().FramePadding.x * 2.0f)))
        {
            static std::random_device rd;
            static std::mt19937 mt(rd());
            std::uniform_int_distribution<unsigned int> seedR(0, std::numeric_limits<unsigned int>::max());
            std::uniform_int_distribution<unsigned int> octavesR(2, 6);
            std::uniform_real_distribution<float> noiseScaleR(10.0f, 50.0f);
            std::uniform_real_distribution<float> offsetR(-10000.0f, 10000.0f);

            data.seed = seedR(mt);
            data.noiseScale = noiseScaleR(mt);
            data.octaves = octavesR(mt);
            data.offset = { offsetR(mt), offsetR(mt) };

            GenerateTerrain();
        }
    }
    if(ImGui::CollapsingHeader("Textures", ImGuiTreeNodeFlags_DefaultOpen))
    {
        ImGui::Text("Height Map");
        ImGui::Image(meshTexture->GetID(), ImVec2(panelSize.x, panelSize.x));
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


void Application::OnEvent(Event& event)
{
    switch (event.GetType())
    {
        case EventType::KeyDown:
        {
            auto& ev = static_cast<KeyDownEvent&>(event);
            OnKeyDown(ev);
            break;
        }
        case EventType::KeyUp:
        {
            auto& ev = static_cast<KeyUpEvent&>(event);
            OnKeyUp(ev);
            break;
        }
        default:
            break;
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
                KeyDownEvent e(event.key.scancode, event.key.mod, event.key.repeat);
                cameraEvents.Dispatch(e);
                appEvents.Dispatch(e);
                break;
            }
            case SDL_EVENT_KEY_UP:
            {
                KeyUpEvent e(event.key.scancode, event.key.mod);
                cameraEvents.Dispatch(e);
                appEvents.Dispatch(e);
                break;
            }
            case SDL_EVENT_MOUSE_MOTION:
            {
                MouseMoveEvent e(event.motion.x, event.motion.y, event.motion.xrel, event.motion.yrel);
                cameraEvents.Dispatch(e);
                break;
            }
            case SDL_EVENT_MOUSE_WHEEL:
            {
                if (viewportHovered)
                {
                    MouseScrollEvent e(event.wheel.x, event.wheel.y);
                    cameraEvents.Dispatch(e);
                }
                break;
            }
            case SDL_EVENT_MOUSE_BUTTON_DOWN:
            {
                MouseButtonDownEvent e(event.button.button, event.button.x, event.button.y);
                cameraEvents.Dispatch(e);
                break;
            }
            case SDL_EVENT_MOUSE_BUTTON_UP:
            {
                MouseButtonUpEvent e(event.button.button, event.button.x, event.button.y);
                cameraEvents.Dispatch(e);
                break;
            }
            default:
                break;
        }
    }
}

void Application::RegisterListeners()
{
    // Register camera events
    cameraEvents.Subscribe(EventType::MouseButtonDown, [this](Event& event) { camera->OnEvent(event); });
    cameraEvents.Subscribe(EventType::MouseButtonUp, [this](Event& event) { camera->OnEvent(event); });
    cameraEvents.Subscribe(EventType::MouseMove, [this](Event& event) { camera->OnEvent(event); });
    cameraEvents.Subscribe(EventType::MouseScroll, [this](Event& event) { camera->OnEvent(event); });
    cameraEvents.Subscribe(EventType::KeyDown, [this](Event& event) { camera->OnEvent(event); });
    cameraEvents.Subscribe(EventType::KeyUp, [this](Event& event) { camera->OnEvent(event); });

    // Register app events
    appEvents.Subscribe(EventType::KeyUp, [this](Event& event) { this->OnEvent(event); });
    appEvents.Subscribe(EventType::KeyDown, [this](Event& event) { this->OnEvent(event); });
}

void Application::GenerateTerrain()
{
    MapGenerator generator;

    auto map = generator.GenerateNoiseMap(noiseType, data.noiseScale, data.octaves, data.persistence, data.lacunarity, data.offset, data.seed);

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

    if (pixelate)
    {
        meshTexture->Create(size, size, pixels.data(), TextureType::Colour, TextureFormat::GS, TextureWrapping::Repeat, TextureFilter::Nearest);
    }
    else
    {
        meshTexture->Create(size, size, pixels.data(), TextureType::Colour, TextureFormat::GS, TextureWrapping::Repeat, TextureFilter::Linear);
    }
}

void Application::Reset()
{
    displayType = DisplayType::HeightMap;
    noiseType = NoiseType::Perlin;
    data.seed = 0;
    data.lacunarity = 2.0f;
    data.noiseScale = 20.0f;
    data.persistence = 0.5f;
    data.octaves = 1;
    data.offset = Vec2::Zero();
    pixelate = false;
    autoUpdateGenerator = false;

    GenerateTerrain();
}

void Application::Open()
{
}

void Application::Save()
{
}

void Application::Export()
{
}

void Application::OnMouseDown(MouseButtonDownEvent& event)
{
}

void Application::OnMouseUp(MouseButtonUpEvent& event)
{
}

void Application::OnMouseScroll(MouseScrollEvent& event)
{
}

void Application::OnMouseMove(MouseMoveEvent& event)
{
}

void Application::OnKeyDown(KeyDownEvent& event)
{
    switch (event.code)
    {
        case SDL_SCANCODE_N:
            if (event.mod == SDL_KMOD_CTRL)
            {
                Reset();
                event.handled = true;
            }
            break;
        case SDL_SCANCODE_O:
            if (event.mod == SDL_KMOD_CTRL)
            {
                Open();
                event.handled = true;
            }
            break;
        case SDL_SCANCODE_S:
            if (event.mod == SDL_KMOD_CTRL)
            {
                Save();
                event.handled = true;
            }
            break;
        default:
            break;
    }

    
}

void Application::OnKeyUp(KeyUpEvent& event)
{

}
