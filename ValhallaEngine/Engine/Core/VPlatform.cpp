// -------------------------------------------------
//
//  MIT License
//
//  VPlatform.cpp
//  Valhalla Engine Editor
//
//  Created by Andrew Skatzes on 1/8/26.
//
// -------------------------------------------------

#include "headers/VPlatform.h"

#if defined(_WIN32) || defined(_WIN64)
    #include <windows.h>
#elif defined(__linux__) 

#endif

namespace ValhallaEngine::Platform 
{
    void InfoMessageBox(const char* BoxTitle, const char* Message, ...)
    {
        char buffer[1024];

        va_list args;
        va_start(args, Message);
        vsnprintf(buffer, sizeof(buffer), Message, args);
        va_end(args);

        #if defined(_WIN32) || defined(_WIN64)
        MessageBoxA(nullptr, buffer, BoxTitle, MB_OK | MB_ICONINFORMATION);
        #elif defined(__linux__)

        #endif
    }

    void ErrorMessageBox(const char* BoxTitle, const char* Message, ...)
    {
        char buffer[1024];

        va_list args;
        va_start(args, Message);
        vsnprintf(buffer, sizeof(buffer), Message, args);
        va_end(args);

        #if defined(_WIN32) || defined(_WIN64)
        MessageBoxA(nullptr, buffer, BoxTitle, MB_OK | MB_ICONWARNING);
        #elif defined(__linux__)

        #endif
    }

    void FatalMessageBox(const char* BoxTitle, const char* Message, ...)
    {
        char buffer[1024];

        va_list args;
        va_start(args, Message);
        vsnprintf(buffer, sizeof(buffer), Message, args);
        va_end(args);   

        #if defined(_WIN32) || defined(_WIN64)
        MessageBoxA(nullptr, buffer, BoxTitle, MB_OK | MB_ICONERROR);
        #elif defined(__linux__)

        #endif
    }
}