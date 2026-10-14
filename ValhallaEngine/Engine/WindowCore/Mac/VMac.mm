// -------------------------------------------------
//
//  MIT License
//
//  v_mac.cpp
//  Valhalla Engine Editor
//
//  Created by Andrew Skatzes on 1/22/26.
//
// -------------------------------------------------

#include "headers/VMac.h"
#import <Foundation/Foundation.h>
#import <AppKit/AppKit.h>

// yes this code was written on a mac

using namespace ValhallaEngine::Mac;

VMacWindow::VMacWindow(uint32 WinWidth, uint32 WinHeight, bool WinVisible, bool WinActive)
{
    WindowWidth         = WinWidth;
    WindowHeight        = WinHeight;
    bIsWindowVisible    = WinVisible;
    bIsActiveWindow     = WinActive;
}

VMacWindow::~VMacWindow()
{
    WindowWidth         = 0;
    WindowHeight        = 0;
    bIsWindowVisible    = false;
    bIsActiveWindow     = false;
}
