/// @file    Player.h
/// @author  Matthew Green
/// @date    2023-11-14 16:40:25
/// 
/// @section LICENSE
/// 
/// Copyright (c) 2023 Matthew Green - All rights reserved
/// Unauthorized copying of this file, via any medium is strictly prohibited
/// Proprietary and confidential

#pragma once

#include <velecs/Math/Vec3.h>

#include <string>

namespace hh {

/// @struct Player
/// @brief Brief description.
///
/// Rest of description.
struct Player {
public:
    // Enums

    // Public Fields

    float baseMovementSpeed{ 2.0f };

    velecs::Vec3 targetCamPos{ 0.0f, 0.0f, 10.0f };
    float camZoomSpeed{ 10.0f }; // m/s
    float camMinZoom{ 2.0f }; // m
    float camMaxZoom{ 25.0f }; // m

    // Constructors and Destructors

    /// @brief Default constructor.
    Player() = default;

    /// @brief Default deconstructor.
    ~Player() = default;

    // Public Methods

protected:
    // Protected Fields

    // Protected Methods

private:
    // Private Fields

    // Private Methods
};

} // namespace hh
