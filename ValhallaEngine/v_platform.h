// -------------------------------------------------
//
//  MIT License
//
//  v_platform.h
//  Valhalla Engine Editor
//
//  Created by Andrew Skatzes on 1/8/26.
//
// -------------------------------------------------

#pragma once

#include <iostream>

namespace ValhallaEngine::Platform
{
    /**
        Displays a infomational message using NSAlert. This is used to let the user know something that is not an error
        - Parameter BoxTitle: The title of the message box
        - Parameter Message: The message that will be displayed to explain something
     */
    void InfoMessageBox(const char* BoxTitle, const char* Message, ...);
    /**
        Displays a Error message using NSAlert, not a critical one
        - Parameter BoxTitle: The title of the message box
        - Parameter Message: The message that will be displayed to explain the error
     */
    void ErrorMessageBox(const char* BoxTitle, const char* Message, ...);

    /**
        Displays an error message using NSAlert, a critical one
        - Note: Only use this WHEN there is a FATAL error that could cause something like data lost otherwise use `ErrorMessageBox`
        - Parameter BoxTitle: The title of the message box
        - Parameter Message: The message that will be displayed to explain the error
     */
    void FatalMessageBox(const char* BoxTitle, const char* Message, ...);
}
