/// @file    Main.cpp
/// @author  Matthew Green
/// @date    10/15/2023 15:30:21
///
/// @section LICENSE
///
/// Copyright (c) 2023 Matthew Green - All rights reserved
/// Unauthorized copying of this file, via any medium is strictly prohibited
/// Proprietary and confidential

#include <velecs/engine/Engine.hpp>
using namespace velecs::engine;

#include <velecs/math/Vec3.hpp>
using namespace velecs::math;

#include <velecs/ecs/Common.hpp>
using namespace velecs::ecs;

#include <velecs/graphics/Components/MeshRenderer.hpp>
#include <velecs/graphics/Components/PerspectiveCamera.hpp>
#include <velecs/graphics/Shader/ShaderPrograms/RasterizationShaderProgram.hpp>
using namespace velecs::graphics;

#define SDL_MAIN_USE_CALLBACKS
#include <SDL3/SDL_main.h>
#include <SDL3/SDL_init.h>

#include <imgui.h>

#include <iostream>
#include <string>

class ImGUIDemoWindow : public System {
protected:
    void ProcessGUI(void* context) override
    {
        // Some imgui UI to test
        ImGui::ShowDemoWindow();
    }
};

class MainScene : public Scene {
public:
    MainScene(const std::string& name, ConstructorKey key)
        : Scene(name, key) {}

private:
    void OnEnter() override
    {
        // TryAddSystem<ImGUIDemoWindow>();

        Entity cameraEntity = CreateEntity()
            .WithName("Main Camera")
            .WithPos(Vec3::BACKWARD * -10.0f);
            ;
        PerspectiveCamera* camera{nullptr};
        cameraEntity.TryAddComponent(camera);

        Entity entity = CreateEntity()
            .WithName("Test")
            .WithPos(Vec3::RIGHT * 1.0f)
            ;
        
        MeshRenderer* renderer{nullptr};
        entity.TryAddComponent(renderer);
        renderer->mesh = Mesh::CreateFrom("internal/meshes/equilateral_triangle.obj");
    }
};

class ShaderCustomTest1 : public RasterizationShaderProgram {
public:
    ShaderCustomTest1()
    {
        SetVertexShader(VertexShader::FromFile("internal/shaders/simple_triangle_test.vert.spv"));
        SetFragmentShader(FragmentShader::FromFile("internal/shaders/simple_triangle_test.frag.spv"));

        pipelineBuilder.SetTopology(VK_PRIMITIVE_TOPOLOGY_TRIANGLE_LIST)
            .SetPolygonMode(VK_POLYGON_MODE_FILL)
            .SetCullMode(VK_CULL_MODE_NONE, VK_FRONT_FACE_COUNTER_CLOCKWISE)
            .SetMultisamplingNone()
            .DisableBlending()
            .DisableDepthTest()
            .SetDepthFormat(VK_FORMAT_UNDEFINED)
            ;
    }
};

SDL_AppResult SDL_AppInit(void **engine, int argc, char** argv)
{
    return Engine::SDL_AppInit(engine, argc, argv, [](Engine& engine) {
        engine.SetCompanyName("Matthew Green")
            .SetAppTitle("Harvest Havoc")
            .SetWindowFullscreen(false)
            .RegisterScene<MainScene>("Main Scene")
            .SetStartingScene("Main Scene")
            .Init()
            // .RegisterRasterizationShaderProgram<ShaderCustomTest1>("Custom/Test1")
            ;
    });
}

SDL_AppResult SDL_AppIterate(void *engine)
{
    return Engine::SDL_AppIterate(engine);
}

SDL_AppResult SDL_AppEvent(void *engine, SDL_Event *event)
{
    return Engine::SDL_AppEvent(engine, event);
}

void SDL_AppQuit(void *engine, SDL_AppResult result)
{
    Engine::SDL_AppQuit(engine, result);
}
