/// @file    Main.cpp
/// @author  Matthew Green
/// @date    10/15/2023 15:30:21
///
/// @section LICENSE
///
/// Copyright (c) 2023 Matthew Green - All rights reserved
/// Unauthorized copying of this file, via any medium is strictly prohibited
/// Proprietary and confidential

// #include <velecs/VelECSEngine.h>

// #include "ECS/ECSManager.h"

#include <velecs/engine/Engine.hpp>
using namespace velecs::engine;

#define SDL_MAIN_USE_CALLBACKS
#include <SDL3/SDL_main.h>
#include <SDL3/SDL_init.h>

#include <iostream>

SDL_AppResult SDL_AppInit(void **engine, int argc, char **argv)
{
    *engine = Engine::Create();
    Engine& engineRef = *static_cast<Engine*>(*engine);
    engineRef.SetTitle("Harvest Havoc")
        .SetWindowFullscreen(false)
        ;
    
    return engineRef.Init();
}

SDL_AppResult SDL_AppIterate(void *engine)
{
    Engine& engineRef = *static_cast<Engine*>(engine);

    engineRef.Update();

    return SDL_AppResult::SDL_APP_CONTINUE;
}

SDL_AppResult SDL_AppEvent(void *engine, SDL_Event *event)
{
    switch (event->type)
    {
    case SDL_EVENT_QUIT:
        return SDL_AppResult::SDL_APP_SUCCESS;
    }

    Engine& engineRef = *static_cast<Engine*>(engine);

    engineRef.ProcessSDLEvent(*event);

    return SDL_AppResult::SDL_APP_CONTINUE;
}

void SDL_AppQuit(void *engine, SDL_AppResult result)
{
    Engine* enginePtr = static_cast<Engine*>(engine);
    enginePtr->Cleanup();
    delete enginePtr;
}
