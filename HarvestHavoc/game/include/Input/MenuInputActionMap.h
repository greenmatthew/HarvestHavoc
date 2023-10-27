/// \file    MenuInputActionMap.h
/// \author  Matthew Green
/// \date    10/16/2023 18:49:47
/// 
/// \section LICENSE
/// 
/// Copyright (c) 2023 Matthew Green - All rights reserved
/// Unauthorized copying of this file, via any medium is strictly prohibited
/// Proprietary and confidential

#pragma once

#include <Input/InputActionMap.h>
#include "Input/Input.h"

namespace hh::Input {

/// \class MenuInputActionMap
/// \brief Manages the mappings of SDL keycodes to input actions for menu interactions.
/// 
/// The MenuInputActionMap class is a specialized subclass of InputActionMap tailored for handling
/// input events within menus. It defines specific input actions related to menu interactions, such as 
/// navigating through menu items, selecting options, and exiting menus. The class provides convenient 
/// access to commonly used menu input actions through public member variables, and overrides methods 
/// from the InputActionMap class to implement menu-specific input handling logic.
class MenuInputActionMap : public engine::InputActionMap {
public:
    // Enums

    // Public Fields
    
    std::shared_ptr<engine::InputAction> Escape; /// \brief The input action associated with the Escape key, typically used to exit a menu.

    // Constructors and Destructors

    /// \brief Default constructor.
    MenuInputActionMap() = default;

    /// \brief Default destructor.
    ~MenuInputActionMap() = default;

    // Public Methods

    /// \brief Initializes the MenuInputActionMap, creating necessary input action bindings.
    void Init() override;

    /// \brief Defines behavior when switching action maps, if necessary for menu interactions.
    void Switch() override;

protected:
    // Protected Fields

    // Protected Methods

private:
    // Private Fields

    // Private Methods
};

} // namespace hh::Input
