/// @file    ECSManager.cpp
/// @author  Matthew Green
/// @date    10/30/2023 16:17:43
/// 
/// @section LICENSE
/// 
/// Copyright (c) 2023 Matthew Green - All rights reserved
/// Unauthorized copying of this file, via any medium is strictly prohibited
/// Proprietary and confidential

#include "ECS/ECSManager.h"

#include <velecs/ECS/Modules/PipelineECSModule.h>

#include <velecs/ECS/Modules/InputECSModule.h>

#include "ECS/Modules/EnvironmentECSModule.h"
#include "ECS/Modules/PlayerECSModule.h"

using namespace velecs;

namespace hh {

// Public Fields

// Constructors and Destructors

ECSManager::ECSManager(velecs::VelECSEngine& engine)
    : IECSManager(engine)
{
    // Required for ability to shutdown the application
    ecs.import<InputECSModule>();

    ecs.import<EnvironmentECSModule>();
    
    ecs.import<PlayerECSModule>();
}

// Public Methods

void ECSManager::Init()
{
    
}

void ECSManager::Cleanup()
{
    
}

bool ECSManager::GetIsQuitting() const
{
    flecs::entity inputEntity = ecs.singleton<Input>();
    if (inputEntity == flecs::entity::null())
    {
        throw std::runtime_error("Missing import of " + std::string(typeid(InputECSModule).name()) + ". Without it, the application cannot close properly.");
    }

    const Input * const input = inputEntity.get<Input>();
    if (input == nullptr)
    {
        throw std::runtime_error("Missing import of " + std::string(typeid(InputECSModule).name()) + ". Without it, the application cannot close properly.");
    }

    return input->isQuitting;
}

// Protected Fields

// Protected Methods

// Private Fields

// Private Methods

} // namespace hh
