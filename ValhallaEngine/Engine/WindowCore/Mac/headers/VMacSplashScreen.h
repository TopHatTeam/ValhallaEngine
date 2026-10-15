// -------------------------------------------------
//
//  MIT License
//
//  VMacSplashScreen.h
//  Valhalla Engine Editor
//
//  Created by Andrew Skatzes on 9/8/26.
//
// -------------------------------------------------

#pragma once
#include "VMac.h"

namespace ValhallaEngine::Mac
{
    struct VMacSplash
    {
    public:

        static void DisplaySplash(const char* filepath);

        static void HideSplash();
        
        static void Desconstruct();
    };

}
