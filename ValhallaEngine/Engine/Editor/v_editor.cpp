// -------------------------------------------------
//
//  MIT License
//
//  v_editor.cpp
//  Valhalla Engine Editor
//
//  Created by Andrew Skatzes on 1/14/26.
//
// -------------------------------------------------

// This is extremely incomplete! Also a shit start

#include "headers/v_editor.hpp"

using namespace ValhallaEngine::Editor;

void StartEditor()
{
    
}

void DisplaySplashScreen()
{
    
}

VEditor3DViewer::VEditor3DViewer()
{
    WindowViewer = new QQuickWindow;
    WindowViewer->setColor(Qt::black);

#if defined(__APPLE__)
    WindowViewer->setGraphicsApi(QSGRendererInterface::Metal);
#else
    WindowViewer->setGraphicsApi(QSGRendererInterface::Vulkan);
#endif
    
    WindowViewer->setPersistentSceneGraph(true);
}

VEditor3DViewer::~VEditor3DViewer()
{
    delete WindowViewer;
}
