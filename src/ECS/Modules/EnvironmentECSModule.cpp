/// @file    EnvironmentECSModule.cpp
/// @author  Matthew Green
/// @date    2023-11-30 14:23:42
/// 
/// @section LICENSE
/// 
/// Copyright (c) 2023 Matthew Green - All rights reserved
/// Unauthorized copying of this file, via any medium is strictly prohibited
/// Proprietary and confidential

#include "ECS/Modules/EnvironmentECSModule.h"

#include <sstream>

using namespace velecs;

namespace hh {

// Public Fields

// Constructors and Destructors

EnvironmentECSModule::EnvironmentECSModule(flecs::world& ecs)
    : IECSModule(ecs)
{
    ecs.import<RenderingECSModule>();
    ecs.import<NametagECSModule>();
    ecs.import<InputECSModule>();

    flecs::entity squarePrefab = Prefab::Find("velecs::RenderingECSModule::PR_SquareRender");

    flecs::entity ironNodeChunkPrefab = Prefab::CreateFromPrefab("PR_IronNode", squarePrefab)
        .override<IronVein>()
        ;
    ironNodeChunkPrefab.get_mut<Material>()->color = Color32::SILVER;

    flecs::entity ironNode = Entity::Create(Vec3::LEFT * 6.0f)
        .add<IronNode>()
        ;
    ironNode.set_name("Iron Node");
    Nametag::AddTo(ecs, ironNode, "Iron Node", Vec3::ZERO);

    flecs::entity test = Entity::CreateFromPrefab(ironNodeChunkPrefab,  Vec3::ZERO, None, None, ironNode);
    std::cout << test.parent().name() << std::endl;
    Entity::CreateFromPrefab(ironNodeChunkPrefab,    Vec3::UP, None, None, ironNode);
    Entity::CreateFromPrefab(ironNodeChunkPrefab,  Vec3::DOWN, None, None, ironNode);
    Entity::CreateFromPrefab(ironNodeChunkPrefab, Vec3::RIGHT, None, None, ironNode);
    Entity::CreateFromPrefab(ironNodeChunkPrefab,  Vec3::LEFT, None, None, ironNode);

    ecs.system()
        .kind(stages->Draw)
        .iter([this](flecs::iter& it)
        {
            flecs::world ecs = it.world();
            
            ShowMaterials(ecs);
        });

    ecs.system<Transform, IronVein>()
        .kind(stages->Draw)
        .iter([this](flecs::iter& it, Transform* transforms, IronVein* ironVeins)
        {
            flecs::world ecs = it.world();
            
            flecs::entity playerEntity = Entity::Find(ecs, "hh::PlayerECSModule::Player");
            const Transform* const playerTransform = playerEntity.get<Transform>();

            for (auto i : it)
            {
                Transform& transform = transforms[i];
                IronVein& ironVein = ironVeins[i];

                const float distance = (playerTransform->GetAbsPosition() - transform.GetAbsPosition()).Magnitude();
                if (distance < 0.5f)
                {
                    ShowInteractMessage();
                    break;
                }
            }
        });
    
    ecs.system<Transform, IronVein>()
        .kind(stages->Update)
        .iter([this](flecs::iter& it, Transform* transforms, IronVein* ironVeins)
        {
            float deltaTime = it.delta_time();
            flecs::world ecs = it.world();

            flecs::entity playerEntity = Entity::Find(ecs, "hh::PlayerECSModule::Player");
            const Transform* const playerTransform = playerEntity.get<Transform>();

            Iron* const iron = ecs.get_mut<Iron>();

            flecs::entity inputEntity = ecs.singleton<Input>();
            const Input* const input = inputEntity.get<Input>();

            for (auto i : it)
            {
                Transform& transform = transforms[i];
                IronVein& ironVein = ironVeins[i];
                flecs::entity entity = it.entity(i);

                const float distance = (playerTransform->GetAbsPosition() - transform.GetAbsPosition()).Magnitude();
                //std::cout << "distance: " << distance << std::endl;
                if (distance < 0.5f)
                {
                    if (input->IsHeld(SDLK_SPACE))
                    {
                        iron->amount += ironVein.Mine(entity, deltaTime);
                    }
                }
            }
        });
}

// Public Methods

// Protected Fields

// Protected Methods

// Private Fields

// Private Methods

void EnvironmentECSModule::ShowMaterials(flecs::world& ecs) const
{
    const Iron* const iron = ecs.get<Iron>();
    uint32_t ironAmount = (iron) ? iron->amount : 0;

    std::ostringstream oss;
    oss << "Materials:\n";
    oss << "Iron: " << ironAmount;

    ImVec2 windowPos = ImVec2(10.0f, 10.0f); // 10 pixels from the top-right corner
    ImVec2 windowPivot = ImVec2(0.0f, 0.0f); // Pivot at the top-right corner of the window

    ImGui::SetNextWindowPos(windowPos, ImGuiCond_Always, windowPivot);
    if (ImGui::Begin("MaterialsMessage", nullptr, ImGuiWindowFlags_NoDecoration | ImGuiWindowFlags_AlwaysAutoResize | ImGuiWindowFlags_NoSavedSettings | ImGuiWindowFlags_NoFocusOnAppearing | ImGuiWindowFlags_NoNav))
    {
        ImGui::Text("%s", oss.str().c_str());
    }
    ImGui::End();
}

void EnvironmentECSModule::ShowInteractMessage() const
{
    const char* message = "Hold SPACE to mine iron";

    // Get window size
    ImVec2 windowSize = ImGui::GetIO().DisplaySize;

    // Calculate the position to place the message at the lower middle part of the screen
    ImVec2 windowPos = ImVec2(windowSize.x * 0.5f, windowSize.y * 0.8f);
    ImVec2 windowPivot = ImVec2(0.5f, 0.5f); // Center the window on the calculated position

    // Begin the ImGui window
    ImGui::SetNextWindowPos(windowPos, ImGuiCond_Always, windowPivot);
    if (ImGui::Begin("InteractMessage", nullptr, ImGuiWindowFlags_NoDecoration | ImGuiWindowFlags_AlwaysAutoResize | ImGuiWindowFlags_NoSavedSettings | ImGuiWindowFlags_NoFocusOnAppearing | ImGuiWindowFlags_NoNav))
    {
        ImGui::Text("%s", message);
    }
    ImGui::End();
}

} // namespace hh
