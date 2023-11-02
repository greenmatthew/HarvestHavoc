/// \file    PlayerInputActionMap.h
/// \author  Matthew Green
/// \date    10/16/2023 18:49:35
/// 
/// \section LICENSE
/// 
/// Copyright (c) 2023 Matthew Green - All rights reserved
/// Unauthorized copying of this file, via any medium is strictly prohibited
/// Proprietary and confidential

#pragma once

#include <Input/InputActionMap.h>
#include "Input/Input.h"

namespace hh {

/// \class PlayerInputActionMap
/// \brief Manages the mappings of SDL keycodes to input actions for player interactions.
///
/// The PlayerInputActionMap class is a specialized subclass of InputActionMap tailored for handling
/// input events related to player actions. It defines specific input actions related to player movement,
/// interaction, and other gameplay mechanics. The class provides convenient access to commonly used player 
/// input actions through public member variables, and overrides methods from the InputActionMap class to 
/// implement player-specific input handling logic.
class PlayerInputActionMap : public velecs::InputActionMap {
public:
    // Enums

    // Public Fields

    std::shared_ptr<velecs::InputAction> Escape; /// \brief The input action associated with the Escape key, typically used to pause the game or access the menu.
    std::shared_ptr<velecs::InputAction> LeftStrafe; /// \brief The input action associated with strafing left.
    std::shared_ptr<velecs::InputAction> RightStrafe; /// \brief The input action associated with strafing right.
    std::shared_ptr<velecs::InputAction> ChangeRenderPipeline;

    // Constructors and Destructors

    /// \brief Default constructor.
    PlayerInputActionMap() = default;

    /// \brief Default destructor.
    ~PlayerInputActionMap() = default;

    // Public Methods

    /// \brief Initializes the PlayerInputActionMap, creating necessary input action bindings.
    void Init() override;

    /// \brief Defines behavior when switching action maps, if necessary for player interactions.
    void Switch() override;
    
protected:
    // Protected Fields

    // Protected Methods

private:
    // Private Fields

    // Private Methods
};

} // namespace hh
