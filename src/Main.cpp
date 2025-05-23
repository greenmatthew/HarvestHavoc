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

#include "velecs/engine2/Engine.hpp"

int main(int argc, char *argv[])
{
    // auto &engine = velecs::VelECSEngine{};

    // engine.SetECS(std::make_unique<hh::ECSManager>(engine))
    //     .Run();

    // return 0;

    auto engine = velecs::engine2::Engine::Create()
        .SetTitle("Harvest Havoc")
        .Init()
        .Run()
        .Cleanup()
        ;
    
    return EXIT_SUCCESS;
}
