/// @file    Nametag.h
/// @author  Matthew Green
/// @date    2023-11-22 17:34:41
/// 
/// @section LICENSE
/// 
/// Copyright (c) 2023 Matthew Green - All rights reserved
/// Unauthorized copying of this file, via any medium is strictly prohibited
/// Proprietary and confidential

#pragma once

#include <velecs/ECS/Modules/RenderingECSModule.h>

#include <imgui.h>

#include <flecs.h>

#include <string>

namespace hh {

/// @struct Nametag
/// @brief Brief description.
///
/// Rest of description.
struct Nametag {
public:
    // Enums

    // Public Fields

    std::string name{"Default Nametag Value"};

    // Constructors and Destructors

    /// @brief Default constructor.
    Nametag() = default;

    Nametag(const flecs::string_view& name);

    Nametag(const std::string& name);

    /// @brief Default deconstructor.
    ~Nametag() = default;

    // Public Methods

    static flecs::entity& AddTo(flecs::world& ecs, flecs::entity entity, const std::string& name, const velecs::Vec3 offset);
    static flecs::entity& AddTo(flecs::world& ecs, flecs::entity entity, const std::string& name = "");
    static flecs::entity& AddTo(flecs::world& ecs, flecs::entity entity, const velecs::Vec3 offset);

    void Display
    (
        const velecs::Transform& transform,
        const velecs::Transform* const cameraTransform,
        const velecs::PerspectiveCamera* const perspectiveCamera,
        const velecs::Vec2 screenRes
    ) const;

protected:
    // Protected Fields

    // Protected Methods

private:
    // Private Fields

    // Private Methods
};

} // namespace hh
