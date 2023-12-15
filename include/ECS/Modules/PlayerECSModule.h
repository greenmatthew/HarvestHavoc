/// @file    PlayerECSModule.h
/// @author  Matthew Green
/// @date    2023-11-14 16:31:02
/// 
/// @section LICENSE
/// 
/// Copyright (c) 2023 Matthew Green - All rights reserved
/// Unauthorized copying of this file, via any medium is strictly prohibited
/// Proprietary and confidential

#pragma once

#include <velecs/ECS/Modules/IECSModule.h>

#include <velecs/ECS/Modules/InputECSModule.h>
#include <velecs/ECS/Modules/RenderingECSModule.h>
#include <velecs/ECS/Modules/PhysicsECSModule.h>

#include "ECS/Modules/NametagECSModule.h"

#include "ECS/Components/Player.h"

#include <velecs/Math/Vec3.h>

#include <string>

namespace hh {

/// @struct PlayerECSModule
/// @brief Brief description.
///
/// Rest of description.
struct PlayerECSModule : public velecs::IECSModule<PlayerECSModule> {
public:
    PlayerECSModule(flecs::world& ecs);

private:
    flecs::entity CreatePlayerEntity
    (
        const std::string& name,
        const velecs::Vec3 position = velecs::Vec3::ZERO,
        const velecs::Vec3 rotation = velecs::Vec3::ZERO
    );

    void HandleInput
    (
        const float deltaTime,
        const velecs::Input* const input,
        velecs::Transform* const cameraTransform,
        Player& player, velecs::Transform& transform,
        velecs::LinearKinematics& linear
    );
};

} // namespace hh
