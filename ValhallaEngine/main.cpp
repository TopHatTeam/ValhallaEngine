// -------------------------------------------------
//
//  MIT License
//
//  main.cpp
//  Valhalla Engine Entry Point
//
//  Created by Andrew Skatzes on 3/29/26.
//
// -------------------------------------------------

#include "Engine/Editor/headers/v_editor.hpp"
#include "main.h"

using namespace ValhallaEngine::Editor;

int main(int argc, char* argv[])
{
	QApplication app(argc, argv);

	QPixmap pixmap(":/assets/VE_SPLASH_02.png");
	if (pixmap.isNull())
	{
		qDebug() << "ERROR: Splash image failed to load";
	}
	QSplashScreen splash(pixmap);
	splash.setWindowFlags(Qt::WindowStaysOnTopHint | Qt::FramelessWindowHint);
	splash.show();
	splash.raise();
	splash.showMessage("Loading Editor...", Qt::AlignHCenter | Qt::AlignBottom);
	QCoreApplication::processEvents();
	QThread::sleep(2);
	splash.showMessage("Starting Editor...", Qt::AlignHCenter | Qt::AlignBottom);
	QCoreApplication::processEvents();
	QThread::sleep(2);
	QMainWindow Window;
	VEditorWindow* Editor = new VEditorWindow(&Window);
	Window.show();
	splash.finish(&Window);
	
	return app.exec();
}
