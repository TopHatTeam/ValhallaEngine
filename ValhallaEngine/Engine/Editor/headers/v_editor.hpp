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
#include <QtGui/QAction>
#include <QtWidgets/QtWidgets>
#include <QtQuick/QQuickWindow>
#include <QtWidgets/QMenuBar>
#include <QtWidgets/QMenu>
#include <QtWidgets/QStatusBar>
#include <QtWidgets/QTabWidget>
#include <QtWidgets/QApplication>


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

    class VEditorWindow : public QMainWindow
    {
        Q_OBJECT
    public:
        VEditorWindow(QMainWindow* MainWindow);
        ~VEditorWindow() override;

        // The ultimate cheat code for C++
        // This is allows us unlimited access!
        // This cannot be safe? Right? 
        friend class VEditor3DViewer;
    protected:
        // --- Menu Bar ---
        QMenuBar* MenuBar;

        // --- Status Bar ---
        QStatusBar* StatusBar;

        // --- Menus ---
        QMenu* ValhallaEngineMenu;
        QMenu* BuildMenu;
        QMenu* EditMenu;
        
        // --- Valhalla Engine Actions ---
        QAction* About;
        QAction* Quit;
        QAction* NewProject;
        QAction* OpenProject;
        QAction* SaveProject;
        QAction* SaveAs;
        QAction* WindowsBuild;
        QAction* MacOSBuild;
        QAction* LinuxBuild;
        QAction* CompileSource;

        // --- Edit Actions ---
        QAction* Copy;
        QAction* Paste;
        QAction* Delete;

        // --- Widgets --- 
        QWidget* CentralWidget;
        QWidget* TabClasses;
        QWidget* TabAssets;

        // --- Tab Widget ---
        QTabWidget* TabEditor;
    public:
        void QuitApplication();
    };

    // --- Variables ---
}
