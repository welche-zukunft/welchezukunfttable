/*
* GuiApp.cpp
*
*  Created on: Oct 28, 2014
*      Author: arturo
*/

#include "GuiApp.h"

void GuiApp::setup() {

	ofSetDataPathRoot("../Resources/data/");

	ofBackground(0,0,0);
	ofSetVerticalSync(false);
	font.load("fonts/open-sans/OpenSans-Regular.ttf", 32);
}

void GuiApp::update() {

}

void GuiApp::draw() {
	ofSetColor(34, 34, 255);
	ofRectRounded(ofMap(*posx, xLeftTop, xRightBottom, 0, ofGetWidth()), ofMap(*posy, yLeftTop, yLeftBottom, 0, ofGetHeight()), 0, 250, 150, 15);
	ofSetColor(255, 0, 0);
	ofCircle(ofMap(*posx, xLeftTop, xRightBottom, 0, ofGetWidth()), ofMap(*posy, yLeftTop, yLeftBottom, 0, ofGetHeight()), 10);
	ofSetColor(255, 255, 255);
	font.drawString("welche zukunft", ofMap(*posx, xLeftTop, xRightBottom, 0, ofGetWidth())+20, ofMap(*posy, yLeftTop, yLeftBottom, 0, ofGetHeight())+20);
}



void GuiApp::keyPressed(int key) {
	/*if (textEdit) {
		if (key == 8) {

			int len = text.length();
			cout << "len: " << len << endl;
			text = text.substr(0, len - 1);
		}
		else { text += key; };
		ofRectangle rect = myfont.getStringBoundingBox(text, 0, 0);
		if (rect.width >= textAreaWidth - 10) {
			int pos = text.find_last_of(" ");
			text.insert(pos, "\n");
		}
	}
	if (headlineEdit) {
		if (key == 8) {

			int len = headline.length();
			cout << "len: " << len << endl;
			headline = headline.substr(0, len - 1);
		}
		else { headline += key; };
		ofRectangle rect = myfont.getStringBoundingBox(text, 0, 0);
		if (rect.width >= textAreaWidth - 10) {
			int pos = headline.find_last_of(" ");
			headline.insert(pos, "\n");
		}
	}*/
}

void GuiApp::mousePressed(int x, int y, int button) {
	/*
	if (button == 0 && x > textAreaStartX && x < textAreaStartX + textAreaWidth && y > textAreaStartY && y < textAreaStartY + textAreaHeight) {
		if (!firstEdit) {
			text = "";
			firstEdit = true;
		}
		textEdit = true;
	}
	else {
		textEdit = false;
	}
	if (button == 0 && x > textAreaStartX && x < textAreaStartX + textAreaWidth && y > textAreaStartY - fontSize - 50 && y < textAreaStartY - 50) {
		if (!firstEditHeadline) {
			headline = "";
			firstEditHeadline = true;
		}
		headlineEdit = true;
	}
	else {
		headlineEdit = false;
	}*/
}
