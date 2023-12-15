/// @file    IronVein.h
/// @author  Matthew Green
/// @date    2023-11-30 20:24:32
/// 
/// @section LICENSE
/// 
/// Copyright (c) 2023 Matthew Green - All rights reserved
/// Unauthorized copying of this file, via any medium is strictly prohibited
/// Proprietary and confidential

#pragma once

#include <velecs/Math/Consts.h>

namespace hh {

/// @struct IronVein
/// @brief Brief description.
///
/// Rest of description.
struct IronVein {
public:
    // Enums

    // Public Fields

    // Constructors and Destructors

    /// @brief Default constructor.
    IronVein() = default;

    /// @brief Default deconstructor.
    ~IronVein() = default;

    // Public Methods

    uint32_t GetAmount() const
    {
        return (uint32_t)std::ceil(amount);
    }

    uint32_t Mine(flecs::entity entity, const float work)
    {
        uint32_t start = GetAmount();
        amount -= work;
        if (amount <= 0.0f)
        {
            entity.destruct();
            flecs::entity entityParent = entity.parent();
            if (entityParent != flecs::entity::null())
            {
                bool deleteParent = true;
                entityParent.children([this, &deleteParent, entity](flecs::entity& child)
                {
                    if (child != entity && child.has<IronVein>())
                    {
                        deleteParent = false;
                        return;
                    }
                });
                if (deleteParent)
                {
                    entityParent.destruct();
                }
            }
            amount = 0.0f;
        }
        uint32_t end = GetAmount();
        
        return start - end;
    }

protected:
    // Protected Fields

    // Protected Methods

private:
    // Private Fields

    float amount{1.0f};

    // Private Methods
};

} // namespace hh
