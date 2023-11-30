/// @file    NametagECSModule.cpp
/// @author  Matthew Green
/// @date    2023-11-30 14:28:33
/// 
/// @section LICENSE
/// 
/// Copyright (c) 2023 Matthew Green - All rights reserved
/// Unauthorized copying of this file, via any medium is strictly prohibited
/// Proprietary and confidential

#include "ECS/Modules/NametagECSModule.h"

using namespace velecs;

namespace hh {

// Public Fields

// Constructors and Destructors

NametagECSModule::NametagECSModule(flecs::world& ecs)
    : IECSModule(ecs)
{
    ecs.import<RenderingECSModule>();

    ecs.component<Nametag>();

    flecs::entity entityPrefab = CommonECSModule::GetPrefab(ecs, "velecs::CommonECSModule::PR_Entity");
    flecs::entity nametagPrefab = ecs.prefab("PR_Nametag")
        .is_a(entityPrefab)
        .add<Nametag>()
        ;

    std::cout << nametagPrefab.path() << std::endl;

    ecs.system<Transform, Nametag>()
        .kind(stages->Draw)
        .iter([this](flecs::iter& it, Transform* transforms, Nametag* nametags)
        {
            float deltaTime = it.delta_time();

            flecs::world ecs = it.world();

            flecs::entity mainCameraEntity = ecs.singleton<MainCamera>();
            flecs::entity cameraEntity = mainCameraEntity.get<MainCamera>()->camera;
            const Transform* const cameraTransform = cameraEntity.get<Transform>();
            const PerspectiveCamera* const perspectiveCamera = cameraEntity.get<PerspectiveCamera>();

            for (auto i : it)
            {
                Transform& transform = transforms[i];
                Nametag& nametag = nametags[i];
                nametag.Display(transform, cameraTransform, perspectiveCamera);
            }
        }
    );
}

// Public Methods

// Protected Fields

// Protected Methods

// Private Fields

// Private Methods

} // namespace hh
