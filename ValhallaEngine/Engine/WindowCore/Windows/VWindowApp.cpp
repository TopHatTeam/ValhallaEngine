// -------------------------------------------------
//
//  MIT License
//
//  VWindowsApp.cpp
//  Valhalla Engine Editor
//
//  Created by Andrew Skatzes on 10/14/26.
//
// -------------------------------------------------

#include <stdio.h>
#include "headers/VWindowsApp.hpp"

using namespace ValhallaEngine::Windows;

int VWindowsApp::RunApplication(HINSTANCE hInstance, int nCmdShow)
{
    // Intitialze the window class
    WNDCLASSEX WindowClass  = { 0 };
    WindowClass.cbSize      = sizeof(WNDCLASSEX);
    WindowClass.style       = CS_HREDRAW | CS_VREDRAW;
    WindowClass.lpfnWndProc = WindowProc;
    WindowClass.hInstance   = hInstance;

    WindowClass.hCursor         = LoadCursor(NULL, IDC_ARROW);
    WindowClass.lpszClassName   = L"VWindowAppClass";
    RegisterClassEx(&WindowClass);

    // We will need a RECT for the window later for resizing and other things
        
}