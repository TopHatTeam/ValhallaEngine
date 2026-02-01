// -------------------------------------------------
//
//  MIT License
//
//  v_mac.hpp
//  Valhalla Engine Editor
//
//  Created by Andrew Skatzes on 1/22/26.
//
// -------------------------------------------------

#pragma once
#include <fmt/core.h>

// no need to do any #if defined(__APPLE__)
// because this should only be compiled on a Mac

#include <CoreFoundation/CoreFoundation.h>
#include <CoreGraphics/CoreGraphics.h>

#include "../../../headers/v_types.hpp"

namespace ValhallaEngine::Mac
{

    // Using an 8-bit value to save resources
    // Along with the fact there's no more than 3 different modes the window could ever be in
    // It would be a waste of RAM to use more than 8-bits of RAM

    /**
        This enum gives 
     */
    enum VMacWindowType : ubyte
    {
        VMWT_FULLSCREEN         = 1,
        VMWT_WINDOWEDFULLSCREEN = 2,
        VMWT_WINDOWED           = 3,
        VMWT_UNDEFINED          = 0
        
    };

    inline VMacWindowType ConvertUbyteToWindowType(ubyte type)
    {
        // Set the default window mode to windowed
        // I'm pretty sure every engine on the planet does this?
        VMacWindowType WinType = VMWT_WINDOWED;
        switch(type)
        {
            case VMWT_FULLSCREEN:
                WinType = VMWT_FULLSCREEN;
                
            case VMWT_WINDOWEDFULLSCREEN:
                WinType = VMWT_WINDOWEDFULLSCREEN;
                
            default:
                WinType = VMWT_WINDOWED;
        }
        
        return WinType;
    }

    inline const char* ConvertWindowTypeToChar(VMacWindowType type)
    {
        switch(type)
        {
            case VMWT_FULLSCREEN:
                return "Fullscreen";
                
            case VMWT_WINDOWED:
                return "Windowed";
                
            case VMWT_WINDOWEDFULLSCREEN:
                return "Windowed Fullscreen";
                
            default:
                return "Undefined";
        }
    }

    class VMacWindow
    {
    public:
        VMacWindow(uint32 WinWidth, uint32 WinHeight, bool WinVisible, bool WinActive);
        ~VMacWindow();
        
    private:
        uint32 WindowHeight;
        uint32 WindowWidth;
        bool bIsWindowVisible;
        bool bIsActiveWindow;
        
    };

    struct VMacSplash 
    {
    public:  

        static void DisplaySplash(const char* filepath);

        static void HideSplash();
    };
}
