#include "ofMain.h"
#include "application.h"


//========================================================================
int main( ){


	//Les commentaires en dessous c'�tait pour l'init par d�faut d'openframeworks

	//Use ofGLFWWindowSettings for more options like multi-monitor fullscreen
	//ofGLWindowSettings settings;
	//settings.setSize(1024, 1024);
	//settings.windowMode = OF_WINDOW; //can also be OF_FULLSCREEN

	//auto window = ofCreateWindow(settings);

	//ofRunApp(window, std::make_shared<ofApp>());
	//ofRunMainLoop();

	ofSetupOpenGL(1024, 1024, OF_WINDOW);
	ofRunApp(new Application());
}
