/// \file    DeletionQueue.cpp
/// \author  Matthew Green
/// \date    10/29/2023 18:19:36
/// 
/// \section LICENSE
/// 
/// Copyright (c) 2023 Matthew Green - All rights reserved
/// Unauthorized copying of this file, via any medium is strictly prohibited
/// Proprietary and confidential

#include "Engine/DeletionQueue.h"

namespace engine {

// Public Fields

// Constructors and Destructors

// Public Methods

void DeletionQueue::PushDeletor(std::function<void()>&& deletor)
{
    deletors.push_back(std::move(deletor));
}

void DeletionQueue::Flush()
{
    // reverse iterate the deletion queue to execute all the functions
    for (auto it = deletors.rbegin(); it != deletors.rend(); it++)
    {
        (*it)(); //call the function
    }

    deletors.clear();
}

// Protected Fields

// Protected Methods

// Private Fields

// Private Methods

} // namespace engine
