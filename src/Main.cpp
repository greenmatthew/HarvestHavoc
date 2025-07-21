/// @file    Main.cpp
/// @author  Matthew Green
/// @date    10/15/2023 15:30:21
///
/// @section LICENSE
///
/// Copyright (c) 2023 Matthew Green - All rights reserved
/// Unauthorized copying of this file, via any medium is strictly prohibited
/// Proprietary and confidential

#include <velecs/engine/Engine.hpp>
using namespace velecs::engine;

#define SDL_MAIN_USE_CALLBACKS
#include <SDL3/SDL_main.h>
#include <SDL3/SDL_init.h>

#include <iostream>

void EntryPoint()
{
    std::cout << "Hello World!" << std::endl;
}

SDL_AppResult SDL_AppInit(void **engine, int argc, char** argv)
{
    *engine = Engine::Create(argc, argv);
    Engine& engineRef = *static_cast<Engine*>(*engine);
    engineRef.SetTitle("Harvest Havoc")
        .SetWindowFullscreen(false)
        .SetEntryPoint(EntryPoint)
        ;

    return Engine::SDL_AppInit(engine, argc, argv);
}

SDL_AppResult SDL_AppIterate(void *engine)
{
    return Engine::SDL_AppIterate(engine);
}

SDL_AppResult SDL_AppEvent(void *engine, SDL_Event *event)
{
    return Engine::SDL_AppEvent(engine, event);
}

void SDL_AppQuit(void *engine, SDL_AppResult result)
{
    Engine::SDL_AppQuit(engine, result);
}
