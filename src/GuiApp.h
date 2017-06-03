#pragma once
#pragma once

#include "ofMain.h"
#include "ofxGui.h"


class GuiApp : public ofBaseApp {
public:
	void setup();
	void update();
	void draw();

	void mousePressed(int x, int y, int button);
	void keyPressed(int key);

	ofParameterGroup parameters;
	ofxPanel gui;
	ofxButton push;

	ofTrueTypeFont font;

	int widthElements;

	float* posx = 0;
	float* posy = 0;


	float xLeftTop = 237.;
	float yLeftTop = 77.;
	float xRightTop = 596.;
	float yRightTop = 61.;
	float xRightBottom = 614.;
	float yRightBottom = 417.;
	float xLeftBottom = 259.;
	float yLeftBottom = 425.;
};

