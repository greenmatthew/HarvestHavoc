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

SDL_AppResult SDL_AppInit(void **engine, int argc, char** argv)
{
    try {
        *engine = Engine::Create(argc, argv);
        Engine& engineRef = *static_cast<Engine*>(*engine);
        engineRef.SetTitle("Harvest Havoc")
            .SetWindowFullscreen(false);
        
        return engineRef.Init();
    }
    catch (const std::exception& e) {
        std::cerr << "Error during initialization: " << e.what() << std::endl;
        return SDL_APP_FAILURE;
    }
    catch (...) {
        std::cerr << "Unknown error during initialization" << std::endl;
        return SDL_APP_FAILURE;
    }
}

SDL_AppResult SDL_AppIterate(void *engine)
{
    try {
        Engine& engineRef = *static_cast<Engine*>(engine);
        engineRef.Update();
        return SDL_APP_CONTINUE;
    }
    catch (const std::exception& e) {
        std::cerr << "Error during update: " << e.what() << std::endl;
        return SDL_APP_FAILURE; // This will cause SDL to quit gracefully
    }
    catch (...) {
        std::cerr << "Unknown error during update" << std::endl;
        return SDL_APP_FAILURE;
    }
}

SDL_AppResult SDL_AppEvent(void *engine, SDL_Event *event)
{
    try {
        switch (event->type)
        {
        case SDL_EVENT_QUIT:
            return SDL_APP_SUCCESS;
        }

        Engine& engineRef = *static_cast<Engine*>(engine);
        engineRef.ProcessSDLEvent(*event);
        return SDL_APP_CONTINUE;
    }
    catch (const std::exception& e) {
        std::cerr << "Error during event processing: " << e.what() << std::endl;
        return SDL_APP_FAILURE;
    }
    catch (...) {
        std::cerr << "Unknown error during event processing" << std::endl;
        return SDL_APP_FAILURE;
    }
}

void SDL_AppQuit(void *engine, SDL_AppResult result)
{
    try
    {
        Engine* enginePtr = static_cast<Engine*>(engine);
        enginePtr->Cleanup();
        delete enginePtr;
    }
    catch (const std::exception& e) {
        std::cerr << "Error during app quit: " << e.what() << std::endl;
    }
    catch (...) {
        std::cerr << "Unknown error during app quit" << std::endl;
    }
}
