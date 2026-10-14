// -------------------------------------------------
//
//  MIT License
//
//  v_level.hpp
//  Valhalla Engine Editor
//
//  Created by Andrew Skatzes on 4/7/26.
//
// -------------------------------------------------

#pragma once

#include "../Core/headers/v_types.hpp"

namespace ValhallaEngine::Level
{
    struct BasicEntity
    {
        char EntityID[16];
        vec3f Position;
        
    };
}
