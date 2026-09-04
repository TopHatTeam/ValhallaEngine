// -------------------------------------------------
//
//  MIT License
//
//  v_platform.cpp
//  Valhalla Engine Editor
//
//  Created by Andrew Skatzes on 1/8/26.
//
// -------------------------------------------------

#include "headers/v_platform.h"

namespace ValhallaEngine::Platform 
{
    void InfoMessageBox(const char* BoxTitle, const char* Message, ...)
    {
        va_list args;
        va_start(args, Message);
        vprintf(Message, args);
        va_end(args);
    }
}