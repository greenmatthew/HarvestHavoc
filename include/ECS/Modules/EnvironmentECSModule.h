/// @file    EnvironmentECSModule.h
/// @author  Matthew Green
/// @date    2023-11-30 14:23:18
/// 
/// @section LICENSE
/// 
/// Copyright (c) 2023 Matthew Green - All rights reserved
/// Unauthorized copying of this file, via any medium is strictly prohibited
/// Proprietary and confidential

#pragma once

#include <velecs/ECS/Modules/IECSModule.h>

#include <velecs/ECS/Modules/RenderingECSModule.h>
#include <velecs/ECS/Modules/InputECSModule.h>

#include "ECS/Modules/NametagECSModule.h"

#include "ECS/Components/Resources/IronNode.h"
#include "ECS/Components/Resources/IronVein.h"
#include "ECS/Components/Resources/Iron.h"

namespace hh {

/// @struct EnvironmentECSModule
/// @brief Brief description.
///
/// Rest of description.
struct EnvironmentECSModule : public velecs::IECSModule<EnvironmentECSModule> {
public:
    // Enums

    // Public Fields

    // Constructors and Destructors

    /// @brief Constructor.
    EnvironmentECSModule(flecs::world& ecs);

    /// @brief Default deconstructor.
    ~EnvironmentECSModule() = default;

    // Public Methods

protected:
    // Protected Fields

    // Protected Methods

private:
    // Private Fields

    // Private Methods

    void ShowMaterials(flecs::world& ecs) const;

    void ShowInteractMessage() const;
};

} // namespace hh
