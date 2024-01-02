/// @file    PlayerECSModule.cpp
/// @author  Matthew Green
/// @date    2023-11-14 16:32:44
/// 
/// @section LICENSE
/// 
/// Copyright (c) 2023 Matthew Green - All rights reserved
/// Unauthorized copying of this file, via any medium is strictly prohibited
/// Proprietary and confidential

#include "ECS/Modules/PlayerECSModule.h"

#include "ECS/ECSManager.h"

#include <algorithm>

#include <imgui.h>

#include <velecs/Math/GLMUtility.h>

using namespace velecs;

namespace hh {

// Public Fields

// Constructors and Destructors

PlayerECSModule::PlayerECSModule(flecs::world& ecs)
    : IECSModule(ecs)
{
    ecs.import<InputECSModule>();
    ecs.import<RenderingECSModule>();
    ecs.import<PhysicsECSModule>();
    ecs.import<NametagECSModule>();

    flecs::entity renderingECSModule = ecs.lookup("velecs::RenderingECSModule");
    const Rect extent = renderingECSModule.get<RenderingECSModule>()->GetWindowExtent();

    flecs::entity trianglePrefab = Prefab::Find("velecs::RenderingECSModule::PR_TriangleRender");
    flecs::entity squarePrefab = Prefab::Find("velecs::RenderingECSModule::PR_SquareRender");

    flecs::entity player = Entity::CreateFromPrefab(trianglePrefab, Vec3::BACKWARD * 0.001f)
        .add<Player>()
        .add<LinearKinematics>()
        ;
    player.set_name("Player");
    player.get_mut<Material>()->color = Color32::GREEN;
    // player.set_override<SimpleMesh>({SimpleMesh::MONKEY()});

    Nametag::AddTo(ecs, player, "Player");

    
    flecs::entity cameraEntity = RenderingECSModule::CreatePerspectiveCamera(ecs, Vec3::BACKWARD * 10.0f, Vec3::ZERO, extent.max.x / extent.max.y);
    cameraEntity.child_of(player);
    ecs.set<MainCamera>({cameraEntity, extent});

    Entity::CreateFromPrefab(squarePrefab, Vec3::UP, Vec3::ZERO, Vec3::ONE * 0.1f);
    Entity::CreateFromPrefab(squarePrefab, Vec3::DOWN, Vec3::ZERO, Vec3::ONE * 0.1f);

    Transform transform;
    transform.scale = Vec3::ONE * 0.1f;
    for (int i = 0; i < 4; ++i)
    {
        transform.position = (i >= 2 ? Vec3::UP : Vec3::DOWN) + (i % 2 == 0 ? Vec3::RIGHT : Vec3::LEFT);
        flecs::entity entity = Nametag::AddTo
        (
            ecs,
            Entity::CreateFromPrefab
            (
                i % 2 == 0 ? trianglePrefab : squarePrefab,
                transform
            ),
            "Entity " + std::to_string(i+1)
        );
        entity.get_mut<Material>()->color = Color32::FromFloat(transform.position.x, transform.position.y, transform.position.z);
    }

    ecs.system<Player, Transform, LinearKinematics>()
        .kind(stages->Update)
        .iter([this](flecs::iter& it, Player* players, Transform* transforms, LinearKinematics* linearKinematics)
        {
            float deltaTime = it.delta_time();

            flecs::world ecs = it.world();
            
            flecs::entity mainCameraEntity = ecs.singleton<MainCamera>();
            flecs::entity cameraEntity = mainCameraEntity.get<MainCamera>()->camera;
            Transform * const cameraTransform = cameraEntity.get_mut<Transform>();

            const Input* const input = ecs.get<Input>();

            for (auto i : it)
            {
                Player& player = players[i];
                Transform& transform = transforms[i];
                LinearKinematics& linear = linearKinematics[i];

                HandleInput(deltaTime, input, cameraTransform, player, transform, linear);
            }
        });
}

// Public Methods

// Protected Fields

// Protected Methods

// Private Fields

// Private Methods

// flecs::entity PlayerECSModule::CreatePlayerEntity
// (
//     const std::string& name,
//     const velecs::Vec3 position /* = velecs::Vec3::ZERO */,
//     const velecs::Vec3 rotation /* = velecs::Vec3::ZERO */
// )
// {
//     auto playerEntity = ECSManager::CreateEntity(ecs(), name, position, rotation);
//     playerEntity.set<Mesh>({})
//         .set<Material>({})
//         .set<LinearKinematics>({})
//         .set<AngularKinematics>({});

//     return playerEntity;
// }

void PlayerECSModule::HandleInput
(
    const float deltaTime,
    const Input* const input,
    Transform* const cameraTransform,
    Player& player,
    Transform& transform,
    LinearKinematics& linear
)
{
    Vec3 velDir = (((input->IsHeld(SDLK_d)) ? Vec3::RIGHT : Vec3::ZERO) +
        ((input->IsHeld(SDLK_a)) ? Vec3::LEFT : Vec3::ZERO) +
        ((input->IsHeld(SDLK_w)) ? Vec3::UP : Vec3::ZERO) +
        ((input->IsHeld(SDLK_s)) ? Vec3::DOWN : Vec3::ZERO)).Normalize();

    linear.velocity = player.baseMovementSpeed * (velDir);


    player.targetCamPos = Vec3
    (
        cameraTransform->position.x,
        cameraTransform->position.y,
        std::clamp(player.targetCamPos.z - input->mouseWheel.y, player.camMinZoom, player.camMaxZoom)
    );
    // Max and min are flipped and negative bc of the coordinate system
    cameraTransform->position = Vec3::Lerp(cameraTransform->position, player.targetCamPos, player.camZoomSpeed * deltaTime);
}

} // namespace hh
