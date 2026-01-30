// -------------------------------------------------
//
//  MIT License
//
//  main.mm
//  Valhalla Engine Editor
//
//  Created by Andrew Skatzes on 12/31/25.
//
// -------------------------------------------------

#import "main.h"
#include "Engine/Editor/headers/v_editor.hpp"

using namespace ValhallaEngine::Editor;

int main(int argc, char* argv[])
{
    QApplication app(argc, argv);
    QPixmap pixmap(":/assets/VE_SPLASH_02.png");
    QSplashScreen splash(pixmap);
    splash.show();
    app.processEvents();
    QMainWindow Window;
    VEditorWindow* Editor = new VEditorWindow(&Window);
    Window.show();
    splash.finish(&Window);
        
    return app.exec();
}

