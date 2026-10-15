// -------------------------------------------------
//
//  MIT License
//
//  VLevel.hpp
//  Valhalla Engine Editor
//
//  Created by Andrew Skatzes on 4/7/26.
//
// -------------------------------------------------

#pragma once

#include "../Core/headers/VTypes.hpp"

namespace ValhallaEngine::Level
{
    struct BasicEntity
    {
        char EntityID[16];
        vec3f Position;
        
    };
}
