//
//  v_level.hpp
//  Valhalla Engine Editor
//
//  Created by Andrew Skatzes on 4/7/26.
//

#ifndef v_level_hpp
#define v_level_hpp

#include "../headers/v_types.hpp"

namespace ValhallaEngine::Level
{
    struct BasicEntity
    {
        char EntityID[16];
        vec3f Position;
        
    };
}

#endif /* v_level_hpp */
