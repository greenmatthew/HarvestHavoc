/// @file    Nametag.cpp
/// @author  Matthew Green
/// @date    2023-11-30 14:39:28
/// 
/// @section LICENSE
/// 
/// Copyright (c) 2023 Matthew Green - All rights reserved
/// Unauthorized copying of this file, via any medium is strictly prohibited
/// Proprietary and confidential

#include "ECS/Components/Nametag.h"

using namespace velecs;

namespace hh {

// Public Fields

// Constructors and Destructors

Nametag::Nametag(const flecs::string_view& name)
    : name(std::string{name}){}

Nametag::Nametag(const std::string& name)
    : name(name) {}

// Public Methods

flecs::entity& Nametag::AddTo(flecs::world& ecs, flecs::entity entity, const std::string& name, const Vec3 offset)
{
    flecs::entity nametagEntity = Entity::Create(ecs, "", offset, Vec3::ZERO, Vec3::ONE, entity);
    if (!name.empty())
    {
        nametagEntity.set_override<Nametag>({name});
    }
    else
    {
        nametagEntity.set_override<Nametag>({entity.name()});
    }

    return entity;
}

flecs::entity& Nametag::AddTo(flecs::world& ecs, flecs::entity entity, const std::string& name /*= ""*/)
{
    Vec3 offset = ((entity.get<Transform>()->scale.y * 0.5f) + 0.1f) * Vec3::UP;

    return AddTo(ecs, entity, name, offset);
}

flecs::entity& Nametag::AddTo(flecs::world& ecs, flecs::entity entity, const Vec3 offset)
{
    return AddTo(ecs, entity, "", offset);
}

void Nametag::Display
(
    const Transform& transform,
    const Transform* const cameraTransform,
    const PerspectiveCamera* const perspectiveCamera,
    const Vec2 screenRes
) const
{
    Vec2 screenPos = transform.GetScreenPosition(cameraTransform, perspectiveCamera);
    if (screenPos.x < 0.0f || screenPos.y < 0.0f || screenPos.x > screenRes.x || screenPos.y > screenRes.y)
    {
        return;
    }

    ImVec2 windowPos = ImVec2(screenPos.x, screenPos.y);
    ImVec2 windowPivotPos = ImVec2(0.5f, 0.5f);
    ImGui::SetNextWindowPos(windowPos, ImGuiCond_Always, windowPivotPos);

    ImGuiWindowFlags windowFlags =
        ImGuiWindowFlags_NoDecoration
        | ImGuiWindowFlags_AlwaysAutoResize
        | ImGuiWindowFlags_NoSavedSettings
        | ImGuiWindowFlags_NoFocusOnAppearing
        | ImGuiWindowFlags_NoNav
        | ImGuiWindowFlags_NoBackground
        ;

    const std::string windowName = name + " Nametag";
    ImGui::Begin(windowName.c_str(), nullptr, windowFlags);
    ImGui::Text(name.c_str());
    ImGui::End();
}

// Protected Fields

// Protected Methods

// Private Fields

// Private Methods

} // namespace hh
