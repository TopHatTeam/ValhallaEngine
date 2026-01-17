// -------------------------------------------------
//
//  MIT License
//
//  v_editor.hpp
//  Valhalla Engine Editor
//
//  Created by Andrew Skatzes on 1/14/26.
//
// -------------------------------------------------

#pragma once
#include <QtCore/QtCore>
#include <QtWidgets/QtWidgets>
#include <QtQuick/QQuickWindow>


namespace ValhallaEngine::Editor
{
    void StartEditor();

    void DisplaySplashScreen();

    class VEditor3DViewer
    {
    public:
        
        VEditor3DViewer();
        
        ~VEditor3DViewer();
    protected:
        QQuickWindow* WindowViewer;
    };

    // --- Variables ---
}
