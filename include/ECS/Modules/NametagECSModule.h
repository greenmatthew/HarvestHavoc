/// @file    NametagECSModule.h
/// @author  Matthew Green
/// @date    2023-11-30 14:28:09
/// 
/// @section LICENSE
/// 
/// Copyright (c) 2023 Matthew Green - All rights reserved
/// Unauthorized copying of this file, via any medium is strictly prohibited
/// Proprietary and confidential

#pragma once

#include <velecs/ECS/Modules/IECSModule.h>

#include <velecs/ECS/Modules/RenderingECSModule.h>

#include "ECS/Components/Nametag.h"

namespace hh {

/// @struct NametagECSModule
/// @brief Brief description.
///
/// Rest of description.
struct NametagECSModule : public velecs::IECSModule<NametagECSModule> {
public:
    // Enums

    // Public Fields

    // Constructors and Destructors

    /// @brief Constructor.
    NametagECSModule(flecs::world& ecs);

    /// @brief Default deconstructor.
    ~NametagECSModule() = default;

    // Public Methods

protected:
    // Protected Fields

    // Protected Methods

private:
    // Private Fields

    // Private Methods
};

} // namespace hh
