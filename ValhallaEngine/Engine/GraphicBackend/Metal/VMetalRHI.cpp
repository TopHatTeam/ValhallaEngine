// -------------------------------------------------
//
//  MIT License
//
//  VMetalRHI.cpp
//  Valhalla Engine Editor
//
//  Created by Andrew Skatzes on 10/15/26.
//
// -------------------------------------------------

#include "headers/VMetalRHI.hpp"

using namespace ValhallaEngine::Metal;

VMacAppDelegate::~VMacAppDelegate()
{
    VViewport->release();
    VWindow->release();
    VDevice->release();
    delete VViewDelegate;
}

NS::Menu* VMacAppDelegate::CreateMenuBar()
{
    using NS::StringEncoding;

    NS::Menu*       VMainMenu       = NS::Menu::alloc()->init();
    NS::MenuItem*   VAppMenuItem    = NS::MenuItem::alloc()->init();
    NS::Menu*       VAppMenu        = NS::Menu::alloc()->init(NS::String::string("ValhallaMetalRHI", StringEncoding::UTF8StringEncoding));

    // it's short for now because most of the things we need we don't have yet

    return VMainMenu->autorelease();
}