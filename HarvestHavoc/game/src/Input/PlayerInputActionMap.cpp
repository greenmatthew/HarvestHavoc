/// \file    PlayerInputActionMap.cpp
/// \author  Matthew Green
/// \date    10/16/2023 18:51:33
/// 
/// \section LICENSE
/// 
/// Copyright (c) 2023 Matthew Green - All rights reserved
/// Unauthorized copying of this file, via any medium is strictly prohibited
/// Proprietary and confidential

#include <Input/InputActionMap.h>
#include <Engine/vk_engine.h>

#include "Input/Input.h"
#include "Input/PlayerInputActionMap.h"
#include "Input/MenuInputActionMap.h"

#include <iostream>

#include "SDL2/SDL.h"

namespace HarvestHavoc::Input {

// Public Fields

// Constructors and Destructors

// Public Methods

void OpenMenu()
{
    std::cout << "Opening menu." << std::endl;
    Input::Menu->Switch();
}

void StartGoingLeft()
{
    std::cout << "Started going left." << std::endl;
}

void StopGoingLeft()
{
    std::cout << "Stopped going left." << std::endl;
}

void StartGoingRight()
{
    std::cout << "Started going right." << std::endl;
}

void StopGoingRight()
{
    std::cout << "Stopped going right." << std::endl;
}

void PlayerInputActionMap::Init()
{
    Escape = CreateBinding(SDLK_ESCAPE);
    Escape->OnPressed += OpenMenu;

    LeftStrafe = CreateBinding(SDLK_LEFT);
    std::function<void()> func = StartGoingLeft;
    LeftStrafe->OnPressed += func;
    LeftStrafe->OnReleased += StopGoingLeft;
    
    RightStrafe = CreateBinding(SDLK_RIGHT);
    RightStrafe->OnPressed += StartGoingRight;
    RightStrafe->OnReleased += StopGoingRight;

    ChangeRenderPipeline = CreateBinding(SDLK_SPACE);
    ChangeRenderPipeline->OnPressed += []() { HarvestHavocEngine::Engine::VulkanEngine::GetInstance().SwapToNextRenderPipeline(); };
}

void PlayerInputActionMap::Switch()
{
    Input::SwitchTo(Input::Player);
}

// Protected Fields

// Protected Methods

// Private Fields

// Private Methods

} // namespace HarvestHavoc::Input
