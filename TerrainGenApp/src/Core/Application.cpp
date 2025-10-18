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
#include <misc/cpp/imgui_stdlib.h>

#include <chrono>
#include <cmath>
#include <iostream>
#include <numbers>

Application* Application::app = nullptr;

static int InputTextResizeCallback(ImGuiInputTextCallbackData* data)
{
    if (data->EventFlag == ImGuiInputTextFlags_CallbackResize)
    {
        std::string str = (const char*)data->UserData;
    }
    return 0;
}

const std::vector<TerrainType> defaultRegions = {
    {"Water - Deep", Vec3(0.0f, 0.0f, 1.0f), 0.0f},
    {"Water", Vec3(0.0f, 0.6f, 1.0f), 0.27f},
    {"Sand", Vec3(0.98f, 0.86f, 0.52f), 0.38f},
    {"Land", Vec3(0.0f, 0.8f, 0.0f), 0.46f},
    {"Land 2", Vec3(0.0f, 0.6f, 0.0f), 0.6f},
    {"Mountain", Vec3(0.25f, 0.25f, 0.25f), 0.8f},
    {"Snow", Vec3(1.0f, 1.0f, 1.0f), 1.0f},
};

bool Application::Initialise()
{
    app = this;

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

    framebuffer = new OpenGL::Framebuffer(1280, 720);

    if (!framebuffer)
    {
        return false;
    }

    rootDir = std::filesystem::current_path();

    camera = new OpenGL::Camera(45.0f, 1.778f, 0.1f, 10000.f);

    heightMap = new OpenGL::Texture;
    noiseMap = new OpenGL::Texture;
    falloffMap = new OpenGL::Texture;
    colourMap = new OpenGL::Texture;

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
    delete falloffMap;
    delete noiseMap;
    delete heightMap;
    delete colourMap;
    delete mat;

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

    mat = new OpenGL::Material(&shader);

    mat->SetTexture(heightMap);

    mesh = new OpenGL::Mesh(mat, MeshShape::Quad);

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
                if (ImGui::MenuItem("FBX"))
                {
                    Export(FileType::FBX);
                }
                if (ImGui::MenuItem("OBJ"))
                {
                    Export(FileType::OBJ);
                }
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

    ////////////////////////////////////////////////////////
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
                        previousDisplayType = displayType;
                        displayType = static_cast<DisplayType>(i);
                    }

                    if (isSelected)
                    {
                        ImGui::SetItemDefaultFocus();
                    }
                }
                ImGui::EndCombo();
            }
            // Display Texture Type Select
            ImGui::TableNextRow();
            ImGui::TableSetColumnIndex(0);
            ImGui::Text("Texture Display Type");
            ImGui::TableSetColumnIndex(1);
            ImGui::SetNextItemWidth(-1.0f);
            if (ImGui::BeginCombo("##displaytexturetype", displayTextureTypeNames[(uint8_t)textureDisplayType]))
            {
                for (int i = 0; i < IM_ARRAYSIZE(displayTextureTypeNames); ++i)
                {
                    const bool isSelected = (textureDisplayType == static_cast<DisplayTextureType>(i));
                    if (ImGui::Selectable(displayTextureTypeNames[i], isSelected))
                    {
                        textureDisplayType = static_cast<DisplayTextureType>(i);
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
                data.noiseScale = 20.0f;
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
            ImGui::DragFloat("##lacunarity", &data.lacunarity, 0.01f, 1.0f, 100.0f, "%.2f");
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
            ////// Height Multiplier //////////////
            ///////////////////////////////////////
            ImGui::TableNextRow();
            ImGui::TableSetColumnIndex(0);
            ImGui::Text("Height Multiplier");
            ImGui::TableSetColumnIndex(1);
            ImGui::SetNextItemWidth(-1.0f);
            ImGui::DragFloat("##heightmultiplier", &data.heightMultiplier, 0.01f, 1.0f, 100.0f, "%.2f");
            ImGui::TableSetColumnIndex(2);
            if (ImGui::Button("Default##heightmultiplier"))
            {
                data.heightMultiplier = 1.0f;
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
                if (autoUpdateGenerator)
                {
                    GenerateTerrain();
                }
            }
            ///////////////////////////////////////
            ////// Falloff Map ////////////////////
            ///////////////////////////////////////
            ImGui::TableNextRow();
            ImGui::TableSetColumnIndex(0);
            ImGui::Text("Use Falloff Map");
            ImGui::TableSetColumnIndex(1);
            if (ImGui::Checkbox("##falloff", &useFalloffMap))
            {
                if(autoUpdateGenerator)
                {
                    GenerateTerrain();
                }
            }

            ImGui::EndTable();
        }

        // Regions /////////////////////////////////
        ImGui::SeparatorText("Regions");
        ///////////////////////////////////////
        ////// Height Curve ///////////////////
        ///////////////////////////////////////
        if (curveEditor.Draw("Height Curve", data.heightCurve))
        {
            if(autoUpdateGenerator)
            {
                GenerateTerrain();
            }
        }

        ImGui::Text("Number of Regions: %i", regions.size());
        for (int i = 0; i < regions.size(); i++)
        {
            if (ImGui::CollapsingHeader((regions[i].name + "##" + std::to_string(i)).c_str(), ImGuiTreeNodeFlags_DefaultOpen))
            {
                if (ImGui::BeginTable(("RegionTable##" + std::to_string(i)).c_str(), 2))
                {
                    ImGui::TableSetupColumn("Name", ImGuiTableColumnFlags_WidthFixed, 80.0f);
                    ImGui::TableSetupColumn("Input", ImGuiTableColumnFlags_WidthStretch, 100.0f);

                    ImGui::TableNextRow();
                    ImGui::TableSetColumnIndex(0);
                    ImGui::Text("Name");
                    ImGui::TableSetColumnIndex(1);
                    ImGui::SetNextItemWidth(-1);
                    ImGui::InputText(("##regionname" + std::to_string(i)).c_str(), &regions[i].name, 0, InputTextResizeCallback);

                    ImGui::TableNextRow();
                    ImGui::TableSetColumnIndex(0);
                    ImGui::Text("Colour");
                    ImGui::TableSetColumnIndex(1);
                    ImGui::SetNextItemWidth(-1);
                    if (ImGui::ColorEdit3(("##regioncolour" + std::to_string(i)).c_str(), &regions[i].colour[0]))
                    {
                        if(autoUpdateGenerator)
                        {
                            GenerateTerrain();
                        }
                    }

                    ImGui::TableNextRow();
                    ImGui::TableSetColumnIndex(0);
                    ImGui::Text("Height");
                    ImGui::TableSetColumnIndex(1);
                    ImGui::SetNextItemWidth(-1);
                    if (ImGui::DragFloat(("##regionheight" + std::to_string(i)).c_str(), &regions[i].height, 0.01f, 0.0f, 1.0f, "%.2f"))
                    {
                        if(autoUpdateGenerator)
                        {
                            GenerateTerrain();
                        }
                    }

                    ImGui::EndTable();
                }
            }
        }

        ImVec2 buttonSize = ImGui::CalcTextSize("Default Regions");
        buttonSize.x += ImGui::GetStyle().FramePadding.x * 2.0f;

        if (ImGui::Button("Add Region", ImVec2(buttonSize.x, 0)))
        {
            TerrainType type("Temp", Vec3(0.0f, 0.0f, 0.0f), 0.0f);
            regions.push_back(type);
            std::sort(regions.begin(), regions.end());
        }
        ImGui::SameLine();
        if (ImGui::Button("Clear Regions", ImVec2(buttonSize.x, 0)))
        {
            regions.clear();
        }
        if (ImGui::Button("Default Regions", ImVec2(buttonSize.x, 0)))
        {
            regions = defaultRegions;
        }
        ImGui::SameLine();
        if (ImGui::Button("Sort Regions", ImVec2(buttonSize.x, 0)))
        {
            std::sort(regions.begin(), regions.end());
        }
        ImGui::Separator();

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
        ImGui::Image(heightMap->GetID(), ImVec2(panelSize.x, panelSize.x));
        ImGui::Text("Noise Map");
        ImGui::Image(noiseMap->GetID(), ImVec2(panelSize.x, panelSize.x));
        ImGui::Text("Colour Map");
        ImGui::Image(colourMap->GetID(), ImVec2(panelSize.x, panelSize.x));
        ImGui::Text("Falloff Map");
        ImGui::Image(falloffMap->GetID(), ImVec2(panelSize.x, panelSize.x));
    }
    ImGui::End();

    // Generator Panel End
    ////////////////////////////////////////////////////////
    // Viewport Start    

    ImGui::Begin("Viewport");
    ImVec2 viewport = ImGui::GetContentRegionAvail();
    viewportHovered = ImGui::IsWindowHovered();
    viewportFocused = ImGui::IsWindowFocused();

    if (OpenGL::FBSpec spec = framebuffer->GetSpec(); viewport.x > 0.0f && viewport.y > 0.0f && ((float)spec.width != viewport.x || (float)spec.height != viewport.y))
    {
        framebuffer->Resize((unsigned int)viewport.x, (unsigned int)viewport.y);
        camera->SetViewportSize(viewport.x, viewport.y);
    }


    ImGui::Image(framebuffer->GetColourTexture()->GetID(), viewport, ImVec2(0, 1), ImVec2(1, 0));
    ImGui::End();

    //Viewport End
    ////////////////////////////////////////////////////////

    if (autoUpdateGenerator)
    {
        if (data != dataLastFrame)
        {
            GenerateTerrain();
            dataLastFrame = data;
        }
    }

    if (displayType != previousDisplayType)
    {
        previousDisplayType = displayType;
        GenerateTerrain();
    }

    switch (textureDisplayType)
    {
        case DisplayTextureType::HeightMap:
            if (useFalloffMap)
            {
                if (mesh->GetMaterial()->GetTexture() != heightMap)
                {
                    mesh->GetMaterial()->SetTexture(heightMap);
                }
            }
            else
            {
                if (mesh->GetMaterial()->GetTexture() != noiseMap)
                {
                    mesh->GetMaterial()->SetTexture(noiseMap);
                }
            }
            
            break;
        case DisplayTextureType::ColourMap:
            if (mesh->GetMaterial()->GetTexture() != colourMap)
            {
                mesh->GetMaterial()->SetTexture(colourMap);
            }
            break;
        default:
            break;
    }

    if (loadedTerrain)
    {
        GenerateTerrain();
        loadedTerrain = false;
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
    if (displayType == DisplayType::Quad)
    {
        model = glm::scale(model, glm::vec3(250.0f));
    }
    else
    {
        model = glm::scale(model, glm::vec3(1.0f));
    }

    shader.SetMat("model", model);

    mesh->Draw();

    framebuffer->Unbind();
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
                if(viewportHovered || viewportFocused)
                {
                    cameraEvents.Dispatch(e);
                }
                appEvents.Dispatch(e);
                break;
            }
            case SDL_EVENT_KEY_UP:
            {
                KeyUpEvent e(event.key.scancode, event.key.mod);
                if (viewportHovered || viewportFocused)
                {
                    cameraEvents.Dispatch(e);
                }
                appEvents.Dispatch(e);
                break;
            }
            case SDL_EVENT_MOUSE_MOTION:
            {
                MouseMoveEvent e(event.motion.x, event.motion.y, event.motion.xrel, event.motion.yrel);
                if (viewportHovered)
                {
                    cameraEvents.Dispatch(e);
                }
                break;
            }
            case SDL_EVENT_MOUSE_WHEEL:
            {
                MouseScrollEvent e(event.wheel.x, event.wheel.y);
                if (viewportHovered)
                {
                    cameraEvents.Dispatch(e);
                }
                break;
            }
            case SDL_EVENT_MOUSE_BUTTON_DOWN:
            {
                MouseButtonDownEvent e(event.button.button, event.button.x, event.button.y);
                if (viewportHovered)
                {
                    cameraEvents.Dispatch(e);
                }
                break;
            }
            case SDL_EVENT_MOUSE_BUTTON_UP:
            {
                MouseButtonUpEvent e(event.button.button, event.button.x, event.button.y);
                if (viewportHovered)
                {
                    cameraEvents.Dispatch(e);
                }
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
    if (regions.empty())
    {
        regions = defaultRegions;
    }

    switch (noiseType)
    {
        case NoiseType::Perlin:
        {
            PerlinGenData perlin(NoiseType::Perlin, data.noiseScale, data.octaves, data.persistence, data.lacunarity, data.offset, data.seed);
            generator.GenerateTerrain(&perlin, regions, data.heightCurve, useFalloffMap, data.heightMultiplier);
            break;
        }
        case NoiseType::Simplex:

            break;
        default:
            break;
    }

    TerrainData terrain = generator.GetData();

    std::vector<OpenGL::Vertex> vertices;

    for (auto& vert : terrain.meshData.vertices)
    {
        glm::vec3 pos = { vert.pos.x, vert.pos.y, vert.pos.z };
        glm::vec3 norm = { vert.norm.x, vert.norm.y, vert.norm.z };
        glm::vec2 uv = { vert.uv.x, vert.uv.y };

        vertices.push_back(OpenGL::Vertex(pos, norm, uv));
    }

    if(mesh) delete mesh;

    switch (displayType)
    {
        case DisplayType::Quad:
            mesh = new OpenGL::Mesh(mat, MeshShape::Quad);
            break;
        case DisplayType::Mesh:
            mesh = new OpenGL::Mesh(vertices, terrain.meshData.indices, mat);
            break;
    }

    if (pixelate)
    {
        heightMap->Create(terrain.size, terrain.size, terrain.heightTexture.data(), TextureType::Colour, TextureFormat::GS, TextureWrapping::ClampToEdge, TextureFilter::Nearest);
        colourMap->Create(terrain.size, terrain.size, terrain.colourTexture.data(), TextureType::Colour, TextureFormat::RGB, TextureWrapping::ClampToEdge, TextureFilter::Nearest);
        falloffMap->Create(terrain.size, terrain.size, terrain.falloffTexture.data(), TextureType::Colour, TextureFormat::GS, TextureWrapping::ClampToEdge, TextureFilter::Nearest);
        noiseMap->Create(terrain.size, terrain.size, terrain.noiseTexture.data(), TextureType::Colour, TextureFormat::GS, TextureWrapping::ClampToEdge, TextureFilter::Nearest);
    }
    else
    {
        heightMap->Create(terrain.size, terrain.size, terrain.heightTexture.data(), TextureType::Colour, TextureFormat::GS, TextureWrapping::ClampToEdge, TextureFilter::Linear);
        colourMap->Create(terrain.size, terrain.size, terrain.colourTexture.data(), TextureType::Colour, TextureFormat::RGB, TextureWrapping::ClampToEdge, TextureFilter::Linear);
        falloffMap->Create(terrain.size, terrain.size, terrain.falloffTexture.data(), TextureType::Colour, TextureFormat::GS, TextureWrapping::ClampToEdge, TextureFilter::Linear);
        noiseMap->Create(terrain.size, terrain.size, terrain.noiseTexture.data(), TextureType::Colour, TextureFormat::GS, TextureWrapping::ClampToEdge, TextureFilter::Linear);
    }
}

void Application::Reset()
{
    displayType = DisplayType::Quad;
    textureDisplayType = DisplayTextureType::HeightMap;
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
    SDL_DialogFileFilter filters[1] = {
        { "Terrain Project", "tgen" }
    };

    SDL_PropertiesID props = SDL_CreateProperties();
    SDL_SetPointerProperty(props, SDL_PROP_FILE_DIALOG_FILTERS_POINTER, &filters);
    SDL_SetNumberProperty(props, SDL_PROP_FILE_DIALOG_NFILTERS_NUMBER, 1);
    SDL_SetPointerProperty(props, SDL_PROP_FILE_DIALOG_WINDOW_POINTER, window);
    SDL_SetStringProperty(props, SDL_PROP_FILE_DIALOG_LOCATION_STRING, (rootDir.string() + '\\').c_str());
    SDL_SetBooleanProperty(props, SDL_PROP_FILE_DIALOG_MANY_BOOLEAN, false);
    SDL_SetStringProperty(props, SDL_PROP_FILE_DIALOG_TITLE_STRING, "Open Terrain Project");
    SDL_SetStringProperty(props, SDL_PROP_FILE_DIALOG_ACCEPT_STRING, "Open");
    SDL_SetStringProperty(props, SDL_PROP_FILE_DIALOG_CANCEL_STRING, "Cancel");

    void* data = 0;

    SDL_ShowFileDialogWithProperties(SDL_FILEDIALOG_OPENFILE, OpenFileCallback, data, props);
}

void Application::Save()
{
    SDL_DialogFileFilter filters[1] = {
        { "Terrain Project", "tgen" }
    };

    std::string defaultName = "Terrain.tgen";

    SDL_PropertiesID props = SDL_CreateProperties();
    SDL_SetPointerProperty(props, SDL_PROP_FILE_DIALOG_FILTERS_POINTER, &filters);
    SDL_SetNumberProperty(props, SDL_PROP_FILE_DIALOG_NFILTERS_NUMBER, 1);
    SDL_SetPointerProperty(props, SDL_PROP_FILE_DIALOG_WINDOW_POINTER, window);
    SDL_SetStringProperty(props, SDL_PROP_FILE_DIALOG_LOCATION_STRING, (rootDir.string() + "\\" + defaultName).c_str());
    SDL_SetBooleanProperty(props, SDL_PROP_FILE_DIALOG_MANY_BOOLEAN, false);
    SDL_SetStringProperty(props, SDL_PROP_FILE_DIALOG_TITLE_STRING, "Save Terrain Project");
    SDL_SetStringProperty(props, SDL_PROP_FILE_DIALOG_ACCEPT_STRING, "Save");
    SDL_SetStringProperty(props, SDL_PROP_FILE_DIALOG_CANCEL_STRING, "Cancel");

    void* data = 0;

    SDL_ShowFileDialogWithProperties(SDL_FILEDIALOG_SAVEFILE, SaveFileCallback, data, props);
}

void Application::Export(FileType type)
{
    SDL_DialogFileFilter filters;

    std::string defaultName;

    currentType = type;

    switch (type)
    {
        case FileType::FBX:
            filters.name = "FBX";
            filters.pattern = "fbx";
            defaultName = "Terrain.fbx";
            break;
        case FileType::OBJ:
            filters.name = "OBJ";
            filters.pattern = "obj";
            defaultName = "Terrain.obj";
            break;
        default:
            break;
    }

    SDL_PropertiesID props = SDL_CreateProperties();
    SDL_SetPointerProperty(props, SDL_PROP_FILE_DIALOG_FILTERS_POINTER, &filters);
    SDL_SetNumberProperty(props, SDL_PROP_FILE_DIALOG_NFILTERS_NUMBER, 1);
    SDL_SetPointerProperty(props, SDL_PROP_FILE_DIALOG_WINDOW_POINTER, window);
    SDL_SetStringProperty(props, SDL_PROP_FILE_DIALOG_LOCATION_STRING, (rootDir.string() + "\\" + defaultName).c_str());
    SDL_SetBooleanProperty(props, SDL_PROP_FILE_DIALOG_MANY_BOOLEAN, false);
    SDL_SetStringProperty(props, SDL_PROP_FILE_DIALOG_TITLE_STRING, "Export Terrain");
    SDL_SetStringProperty(props, SDL_PROP_FILE_DIALOG_ACCEPT_STRING, "Export");
    SDL_SetStringProperty(props, SDL_PROP_FILE_DIALOG_CANCEL_STRING, "Cancel");

    void* data = 0;

    SDL_ShowFileDialogWithProperties(SDL_FILEDIALOG_SAVEFILE, ExportModelCallback, data, props);
}

void ExportModelCallback(void* userdata, const char* const* filelist, int filter_index)
{
    if (!filelist) return;
    else if (filelist[0] == nullptr) return;
    else
    {
        const char* file = *filelist;
        std::cout << file << '\n';

        Application::Get()->ExportFile((void*)file);
    }
}

void SaveFileCallback(void* userdata, const char* const* filelist, int filter_index)
{
    if (!filelist) return;
    else if (filelist[0] == nullptr) return;
    else
    {
        const char* file = *filelist;
        std::cout << file << '\n';

        Application::Get()->SaveFile((void*)file);
    }
}

void SDLCALL OpenFileCallback(void* userdata, const char* const* filelist, int filter_index)
{
    if (!filelist) return;
    else if (filelist[0] == nullptr) return;
    else
    {
        const char* file = *filelist;

        Application::Get()->LoadFile((void*)file);
    }
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

void Application::ExportFile(void* data)
{
    std::string path = (const char*)data;

    std::filesystem::path savePath(path);

    auto terrainData = generator.GetData();

    exporter.Export(savePath, terrainData, currentType);
}

void Application::SaveFile(void* data) const
{
    std::string path = (const char*)data;

    std::filesystem::path savePath(path);

    SaveData saveData;
    saveData.regions = regions;
    saveData.heightCurve = this->data.heightCurve;
    saveData.heightMultiplier = this->data.heightMultiplier;
    saveData.lacunarity = this->data.lacunarity;
    saveData.noiseScale = this->data.noiseScale;
    saveData.octaves = this->data.octaves;
    saveData.persistence = this->data.persistence;
    saveData.offset = this->data.offset;
    saveData.seed = this->data.seed;

    Serializer::Serialize(saveData, savePath);
}

void Application::LoadFile(void* data)
{

    std::string path = (const char*)data;

    std::filesystem::path loadPath(path);

    auto loadData = Serializer::Deserialize(loadPath);

    if(loadData.valid)
    {
        regions = loadData.regions;
        this->data.heightCurve = loadData.heightCurve;
        this->data.heightMultiplier = loadData.heightMultiplier;
        this->data.lacunarity = loadData.lacunarity;
        this->data.noiseScale = loadData.noiseScale;
        this->data.octaves = loadData.octaves;
        this->data.persistence = loadData.persistence;
        this->data.offset = loadData.offset;
        this->data.seed = loadData.seed;

        loadedTerrain = true;
    }
}
