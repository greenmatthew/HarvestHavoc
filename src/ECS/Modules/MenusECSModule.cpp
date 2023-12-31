/// @file    MenusECSModule.cpp
/// @author  Matthew Green
/// @date    2023-12-27 23:10:10
/// 
/// @section LICENSE
/// 
/// Copyright (c) 2023 Matthew Green - All rights reserved
/// Unauthorized copying of this file, via any medium is strictly prohibited
/// Proprietary and confidential

#include "ECS/Modules/MenusECSModule.h"

#include <velecs/ECS/Modules/InputECSModule.h>
#include <velecs/ECS/Modules/RenderingECSModule.h>

#include <imgui_internal.h>
#include <backends/imgui_impl_sdl2.h>
#include <backends/imgui_impl_vulkan.h>

using namespace velecs;

namespace hh {

// Public Fields

// Constructors and Destructors

MenusECSModule::MenusECSModule(flecs::world& ecs)
    : IECSModule(ecs)
{
    ecs.import<InputECSModule>();
    ecs.import<RenderingECSModule>();

    ecs.system()
        .kind(stages->Draw)
        .iter([this](flecs::iter& it)
            {
                // DisplayInventoryAndCraftingMenus();
            }
        );
}

// Public Methods

// Protected Fields

// Protected Methods

// Private Fields

// Private Methods

void MenusECSModule::DisplayInventoryAndCraftingMenus() const
{
    // Ensure the docking feature is enabled
    ImGuiIO& io = ImGui::GetIO();
    io.ConfigFlags |= ImGuiConfigFlags_DockingEnable;
    io.IniFilename = NULL;  // Disable ini saving

    const ImGuiWindowFlags windowFlags =
        ImGuiWindowFlags_NoMove |
        ImGuiWindowFlags_NoSavedSettings |
        ImGuiWindowFlags_NoNav |
        ImGuiWindowFlags_NoDecoration
        ;

    const float windowScale = 0.8f; // 80% of the screen size
    const ImVec2 windowSize = ImVec2(io.DisplaySize.x * windowScale, io.DisplaySize.y * windowScale);
    const ImVec2 windowCenter = ImVec2(io.DisplaySize.x * 0.5f, io.DisplaySize.y * 0.5f); // Center of the screen
    const ImVec2 windowPos = ImVec2(windowCenter.x - windowSize.x * 0.5f, windowCenter.y - windowSize.y * 0.5f); // Adjusted for window size

    const ImVec2 displaySize = io.DisplaySize;
    static ImVec2 prevDisplaySize = displaySize;

    ImGui::SetNextWindowSize(windowSize, ImGuiCond_Always);
    ImGui::SetNextWindowPos(windowPos, ImGuiCond_Always);

    ImGui::Begin("MyWindow", nullptr, windowFlags);

    const ImGuiID dockspaceId = ImGui::GetID("MyDockSpace");
    if (ImGui::DockBuilderGetNode(dockspaceId) && (prevDisplaySize.x != displaySize.x || prevDisplaySize.y != displaySize.y))
    {
        ImGui::DockBuilderRemoveNode(dockspaceId);
        std::cout << "Deleting dockspace" << std::endl;
    }

    if (!ImGui::DockBuilderGetNode(dockspaceId))
    {
        std::cout << "Creating dockspace" << std::endl;
        ImGui::DockSpace(dockspaceId, ImVec2(0.0f, 0.0f), ImGuiDockNodeFlags_None);

        ImGuiID leftDockWindowId, rightDockWindowId;
        ImGui::DockBuilderSplitNode(dockspaceId, ImGuiDir_Left, 0.5f, &leftDockWindowId, &rightDockWindowId);

        ImGuiDockNode* pLeftNode = ImGui::DockBuilderGetNode(leftDockWindowId);
        ImGuiDockNode* pRightNode = ImGui::DockBuilderGetNode(rightDockWindowId);
        pLeftNode->LocalFlags |= ImGuiDockNodeFlags_NoTabBar | ImGuiDockNodeFlags_NoDockingOverMe | ImGuiDockNodeFlags_NoDockingSplit | ImGuiDockNodeFlags_NoResize;
        pRightNode->LocalFlags |= ImGuiDockNodeFlags_NoTabBar | ImGuiDockNodeFlags_NoDockingOverMe | ImGuiDockNodeFlags_NoDockingSplit | ImGuiDockNodeFlags_NoResize;

        // Dock windows
        ImGui::DockBuilderDockWindow("Left Window", leftDockWindowId);
        ImGui::DockBuilderDockWindow("Right Window", rightDockWindowId);

        ImGui::DockBuilderFinish(dockspaceId);

        prevDisplaySize = displaySize;
    }
    else
    {
        ImGui::DockSpace
        (
            dockspaceId,
            ImVec2(0.0f, 0.0f),
            ImGuiDockNodeFlags_NoDockingSplit |
            ImGuiDockNodeFlags_NoResize | 
            ImGuiDockNodeFlags_NoUndocking
        );
    }
    ImGui::End();

    if (ImGui::Begin("Left Window"))
    {
        DisplayInventoryMenu(io);
    }
    ImGui::End();

    if (ImGui::Begin("Right Window"))
    {
        DisplayCraftingMenu(io);
    }
    ImGui::End();
}

void MenusECSModule::DisplayInventoryMenu(const ImGuiIO& io) const
{
    const std::string windowTitle = "Inventory";
    const unsigned int inventorySlots = 48;
    const ImVec2 buttonSize = ImVec2(48, 48);
    const ImVec4 buttonColor = ImVec4(0.2f, 0.2f, 0.2f, 1.0f);

    const ImVec2 windowSize = ImGui::GetWindowSize();
    const ImVec2 contentSize = ImGui::GetContentRegionAvail();
    const ImVec2 itemSpacing = ImGui::GetStyle().ItemSpacing;
    const ImVec2 windowPadding = ImGui::GetStyle().WindowPadding;


    const float titleWidth = ImGui::CalcTextSize(windowTitle.c_str()).x + (itemSpacing.x * 0.5f);
    ImGui::SetCursorPosX((windowSize.x - titleWidth) * 0.5f);
    ImGui::Text("%s", windowTitle.c_str());
    ImGui::Separator();

    const unsigned int maxSlotsHorizontally = static_cast<unsigned int>(std::floorf(contentSize.x / (buttonSize.x + itemSpacing.x)));
    const float totalButtonsWidth = maxSlotsHorizontally * (buttonSize.x + itemSpacing.x) - itemSpacing.x;  // Width of all buttons and the spacing in between
    const float horizontalBuffering = ((contentSize.x + itemSpacing.x + windowPadding.x ) - totalButtonsWidth) * 0.5f;  // Space on each side

    unsigned int currColumn = 1;
    unsigned int currRow = 1;
    ImGui::SetCursorPosX(horizontalBuffering);
    for (int slotIndex = 0; slotIndex < inventorySlots; ++slotIndex)
    {
        ImGui::PushID(slotIndex);

        ImGui::PushStyleColor(ImGuiCol_Button, buttonColor);
        if (ImGui::Button("##Cell", buttonSize))
        {
            // Handle cell click event
        }
        ImGui::PopStyleColor();

        ImGui::PopID();
        if (currColumn++ < maxSlotsHorizontally)
        {
            ImGui::SameLine();
        }
        else
        {
            currColumn = 1;
            ++currRow;
            ImGui::SetCursorPosX(horizontalBuffering);
        }
    }
}

void MenusECSModule::DisplayCraftingMenu(const ImGuiIO& io) const
{
    const std::string windowTitle = "Crafting";
    const unsigned int inventorySlots = 3;
    const ImVec2 buttonSize = ImVec2(48, 48);
    const ImVec4 buttonColor = ImVec4(0.2f, 0.2f, 0.2f, 1.0f);

    const ImVec2 windowSize = ImGui::GetWindowSize();
    const ImVec2 contentSize = ImGui::GetContentRegionAvail();
    const ImVec2 itemSpacing = ImGui::GetStyle().ItemSpacing;
    const ImVec2 windowPadding = ImGui::GetStyle().WindowPadding;


    const float titleWidth = ImGui::CalcTextSize(windowTitle.c_str()).x + (itemSpacing.x * 0.5f);
    ImGui::SetCursorPosX((windowSize.x - titleWidth) * 0.5f);
    ImGui::Text("%s", windowTitle.c_str());
    ImGui::Separator();

    const unsigned int maxSlotsHorizontally = static_cast<unsigned int>(std::floorf(contentSize.x / (buttonSize.x + itemSpacing.x)));
    const float totalButtonsWidth = maxSlotsHorizontally * (buttonSize.x + itemSpacing.x) - itemSpacing.x;  // Width of all buttons and the spacing in between
    const float horizontalBuffering = ((contentSize.x + itemSpacing.x + windowPadding.x ) - totalButtonsWidth) * 0.5f;  // Space on each side

    unsigned int currColumn = 1;
    unsigned int currRow = 1;
    ImGui::SetCursorPosX(horizontalBuffering);
    for (int slotIndex = 0; slotIndex < inventorySlots; ++slotIndex)
    {
        ImGui::PushID(slotIndex);

        ImGui::PushStyleColor(ImGuiCol_Button, buttonColor);
        if (ImGui::Button("##Cell", buttonSize))
        {
            // Handle cell click event
        }
        ImGui::PopStyleColor();

        ImGui::PopID();
        if (currColumn++ < maxSlotsHorizontally)
        {
            ImGui::SameLine();
        }
        else
        {
            currColumn = 1;
            ++currRow;
            ImGui::SetCursorPosX(horizontalBuffering);
        }
    }
}

} // namespace hh
