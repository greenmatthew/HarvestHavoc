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
using namespace velecs::graphics;

#define SDL_MAIN_USE_CALLBACKS
#include <SDL3/SDL_main.h>
#include <SDL3/SDL_init.h>

#include <iostream>

void EntryPoint()
{
    auto& registry = Registry::Get();

    Entity cameraEntity = Entity::Create()
        .WithName("Main Camera")
        .WithPos(Vec3::BACKWARD * -10.0f);
        ;
    auto& camera = cameraEntity.AddComponent<PerspectiveCamera>();
    camera.GetViewMatrix();
    camera.GetProjectionMatrix();
    std::cout << "Camera's name: " << camera.GetOwner().GetName() << std::endl;

    
    auto view = registry.view<Camera>();
    std::cout << "Number of cameras in the scene: " << std::distance(view.begin(), view.end()) << std::endl;
    std::cout << "Is there cameras in the scene: " << (view.begin() != view.end()) << std::endl;
    auto view2 = registry.view<PerspectiveCamera>();
    std::cout << "Number of perspective cameras in the scene: " << std::distance(view2.begin(), view2.end()) << std::endl;
    std::cout << "Is there perspective cameras in the scene: " << (view2.begin() != view2.end()) << std::endl;

    Entity entity = Entity::Create()
        .WithName("Test")
        .WithPos(Vec3::RIGHT * 1.0f)
        ;
    
    auto& renderer = entity.AddComponent<MeshRenderer>();
    renderer.mesh = Mesh::CreateFrom("internal/meshes/equilateral_triangle.obj");
}

SDL_AppResult SDL_AppInit(void **engine, int argc, char** argv)
{
    return Engine::SDL_AppInit(engine, argc, argv, [](Engine& engine) {
        engine.SetTitle("Harvest Havoc")
            .SetWindowFullscreen(false)
            .SetEntryPoint(EntryPoint)
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
