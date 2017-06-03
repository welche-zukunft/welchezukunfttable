#include "ofApp.h"

int main() {
   /* ofSetupOpenGL(1024, 768, OF_WINDOW);
	ofRunApp(new ofApp());*/

	ofGLFWWindowSettings settings;


	// Kalibbrierungsvariablen



	settings.width = 800;
	settings.height = 700;
	settings.setPosition(ofVec2f(40, 40));
	//settings.resizable = true;
	shared_ptr<ofAppBaseWindow> mainWindow = ofCreateWindow(settings);

	settings.width = 640;
	settings.height = 630;
	settings.setPosition(ofVec2f(640+1920, 450));
	//settings.resizable = false;
	shared_ptr<ofAppBaseWindow> guiWindow = ofCreateWindow(settings);

	shared_ptr<ofApp> mainApp(new ofApp);
	shared_ptr<GuiApp> guiApp(new GuiApp);
	float xStart = 0;
	float yStart = 0;
	mainApp->posx = &xStart;
	mainApp->posy = &yStart;
	guiApp->posx = mainApp->posx;
	guiApp->posy = mainApp->posy;
	

	ofRunApp(guiWindow, guiApp);
	ofRunApp(mainWindow, mainApp);
	ofRunMainLoop();
}
