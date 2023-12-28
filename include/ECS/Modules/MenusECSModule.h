/// @file    MenusECSModule.h
/// @author  Matthew Green
/// @date    2023-12-27 23:08:15
/// 
/// @section LICENSE
/// 
/// Copyright (c) 2023 Matthew Green - All rights reserved
/// Unauthorized copying of this file, via any medium is strictly prohibited
/// Proprietary and confidential

#pragma once

#include <velecs/ECS/Modules/IECSModule.h>

#include <imgui.h>

namespace hh {

/// @struct MenusECSModule
/// @brief Brief description.
///
/// Rest of description.
struct MenusECSModule : velecs::IECSModule<MenusECSModule> {
public:
    // Enums

    // Public Fields

    // Constructors and Destructors

    /// @brief Default constructor.
    MenusECSModule(flecs::world& ecs);

    /// @brief Default deconstructor.
    ~MenusECSModule() = default;

    // Public Methods

protected:
    // Protected Fields

    // Protected Methods

private:
    // Private Fields

    // Private Methods

    void DisplayInventoryAndCraftingMenus() const;

    void DisplayInventoryMenu(const ImGuiIO& io) const;
    void DisplayCraftingMenu(const ImGuiIO& io) const;
};

} // namespace hh
