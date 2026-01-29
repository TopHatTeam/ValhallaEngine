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
    // Place holder
}

void DisplaySplashScreen()
{
    // Placeholder
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


VEditorWindow::VEditorWindow(QMainWindow* MainWindow)
{
    if (MainWindow->objectName().isEmpty())
    {
        MainWindow->setObjectName("Valhalla Engine Editor");
    }
    MainWindow->resize(1920, 1080);

    // --- Valhalla Engine Actions ---
    About           = new QAction(MainWindow);
    Quit            = new QAction(MainWindow);
    NewProject      = new QAction(MainWindow);
    OpenProject     = new QAction(MainWindow);
    SaveProject     = new QAction(MainWindow);
    SaveAs          = new QAction(MainWindow);
    WindowsBuild    = new QAction(MainWindow);
    MacOSBuild      = new QAction(MainWindow);
    LinuxBuild      = new QAction(MainWindow);
    CompileSource   = new QAction(MainWindow);
    
    // --- Edit Actions ---
    Copy    = new QAction(MainWindow);
    Paste   = new QAction(MainWindow);
    Delete  = new QAction(MainWindow);

    // --- Set Object Names --- 
    About->setObjectName("About");
    Quit->setObjectName("Quit");
    NewProject->setObjectName("NewProject");
    OpenProject->setObjectName("OpenProject");
    SaveProject->setObjectName("SaveProject");
    SaveAs->setObjectName("SaveAs");
    WindowsBuild->setObjectName("WindowsBuild");
    MacOSBuild->setObjectName("MacOSBuild");
    LinuxBuild->setObjectName("LinuxBuild");
    CompileSource->setObjectName("CompileSource");
    Copy->setObjectName("Copy");
    Paste->setObjectName("Paste");
    Delete->setObjectName("Delete");

    // --- Set up menu bar, along with menus ---
    MenuBar             = new QMenuBar(MainWindow);
    StatusBar           = new QStatusBar(MainWindow);
    ValhallaEngineMenu  = new QMenu(MenuBar);
    BuildMenu           = new QMenu(ValhallaEngineMenu);
    EditMenu            = new QMenu(MenuBar);

    MenuBar->setObjectName("MenuBar");
    StatusBar->setObjectName("StatusBar");
    ValhallaEngineMenu->setObjectName("ValhallaEngineMenu");
    BuildMenu->setObjectName("BuildMenu");
    EditMenu->setObjectName("EditMenu");

    MainWindow->setMenuBar(MenuBar);
    MainWindow->setStatusBar(StatusBar);

    MenuBar->addAction(ValhallaEngineMenu->menuAction());
    MenuBar->addAction(EditMenu->menuAction());

    // --- Implementing the menus for the Valhalla section ---
    ValhallaEngineMenu->addAction(About);
    ValhallaEngineMenu->addAction(Quit);
    ValhallaEngineMenu->addSeparator();
    ValhallaEngineMenu->addAction(NewProject);
    ValhallaEngineMenu->addAction(OpenProject);
    ValhallaEngineMenu->addAction(SaveProject);
    ValhallaEngineMenu->addAction(SaveAs);
    ValhallaEngineMenu->addAction(BuildMenu->menuAction());
    ValhallaEngineMenu->addAction(CompileSource);
    BuildMenu->addAction(WindowsBuild);
    BuildMenu->addAction(MacOSBuild);
    BuildMenu->addAction(LinuxBuild);

    // --- Implementing the menus for the Edit section ---
    EditMenu->addAction(Copy);
    EditMenu->addAction(Paste);
    EditMenu->addAction(Delete);
    
    // --- Implementing the main window title ---
    MainWindow->setWindowTitle("Valhalla Engine Editor");

    // --- Implementing the names for these actions ---
    About->setText(QCoreApplication::translate("Valhalla Engine Editor", "About", nullptr));
    Quit->setText(QCoreApplication::translate("Valhalla Engine Editor", "Quit", nullptr));
    NewProject->setText(QCoreApplication::translate("Valhalla Engine Editor", "New Project", nullptr));
    OpenProject->setText(QCoreApplication::translate("Valhalla Engine Editor", "Open Project", nullptr));
    SaveProject->setText(QCoreApplication::translate("Valhalla Engine Editor", "Save Project", nullptr));
    SaveAs->setText(QCoreApplication::translate("Valhalla Engine Editor", "Save As", nullptr));
    WindowsBuild->setText(QCoreApplication::translate("Valhalla Engine Editor", "Windows Build", nullptr));
    MacOSBuild->setText(QCoreApplication::translate("Valhalla Engine Editor", "MacOS Build", nullptr));
    LinuxBuild->setText(QCoreApplication::translate("Valhalla Engine Editor", "Linux Build", nullptr));
    CompileSource->setText(QCoreApplication::translate("Valhalla Engine Editor", "Compile Source", nullptr));
    Copy->setText(QCoreApplication::translate("Valhalla Engine Editor", "Copy", nullptr));
    Paste->setText(QCoreApplication::translate("Valhalla Engine Editor", "Paste", nullptr));
    Delete->setText(QCoreApplication::translate("Valhalla Engine Editor", "Delete", nullptr));
    ValhallaEngineMenu->setTitle(QCoreApplication::translate("Valhalla Engine Editor", "Valhalla Engine", nullptr));
    BuildMenu->setTitle(QCoreApplication::translate("Valhalla Engine Editor", "Build", nullptr));
    EditMenu->setTitle(QCoreApplication::translate("Valhalla Engine Editor", "Edit", nullptr));
    
    // --- Actions functionality ---
    connect(Quit, &QAction::triggered, this, &VEditorWindow::QuitApplication);
}

VEditorWindow::~VEditorWindow()
{
    // --- Cleaning up Menu bar ---
    delete MenuBar;

    // --- Cleaning up Status Bar ---
    delete StatusBar;

    // --- Cleaning Menu options ---
    delete ValhallaEngineMenu;
    delete BuildMenu;
    delete EditMenu;

    // --- Cleaning up Valhalla Engine actions ---
    delete About;
    delete Quit;
    delete NewProject;
    delete OpenProject;
    delete SaveProject;
    delete SaveAs;
    delete WindowsBuild;
    delete MacOSBuild;
    delete LinuxBuild;
    delete CompileSource;

    // --- Cleaning up Edit actions --- 
    delete Copy;
    delete Paste;
    delete Delete;

    // --- Cleaning up Widgets ---
    delete CentralWidget;
    delete TabClasses;
    delete TabAssets;

    // --- Cleaning up Tab Widget ---
    delete TabEditor;
}

void VEditorWindow::QuitApplication()
{
    QCoreApplication::quit();
}

