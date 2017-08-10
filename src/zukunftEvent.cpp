#include "zukunftEvent.h"

zukunftBlock::zukunftBlock() {

}

void zukunftBlock::setup(ofPoint p, float x, float y, string name) {
	position = p;
	sizex = x;
	sizey = y;
	color.set(ofRandom(255), ofRandom(255), ofRandom(255));
	content = name;
	image.load("img/tank.png");
	texture.allocate(100, 50, GL_RGBA);
	plane.set(100, 50);
	plane.setResolution(2, 2);
	plane.mapTexCoords(image.getWidth(), image.getHeight(), 0, 0);


}

void zukunftBlock::update() {
	image.update();
}

void zukunftBlock::draw() {
	ofSetColor(color);

	//ofRectRounded(position, sizex, sizey, 2);
	roundedRect(position.x, position.y, position.z, sizex, sizey, 2);
	ofPoint contactLine = position;
	contactLine.y = 0.;
	ofDrawLine(position, contactLine);
	ofSetColor(255, 255, 255);
	ofDrawBitmapString(content, position);

	if (position.y >= 0) overUnder = 1;
	else { overUnder = -1; }
	plane.setPosition(position.x, ofMap(abs(position.y), 0, ofGetHeight() / 2, imageAreaTop*overUnder, imageAreaBottom*overUnder, true), offset.z*overUnder);
	ofEnableAlphaBlending();
	image.bind();
	plane.draw();
	image.unbind();
	ofDisableAlphaBlending();


}


void zukunftBlock::roundedRect(float x, float y, float z, float w, float h, float r) {
	ofBeginShape();
	ofVertex(x + r, y, z);
	ofVertex(x + w - r, y, z);
	quadraticBezierVertex(x + w, y, x + w, y + r, z, x + w - r, y);
	ofVertex(x + w, y + h - r, z);
	quadraticBezierVertex(x + w, y + h, x + w - r, y + h, z, x + w, y + h - r);
	ofVertex(x + r, y + h, z);
	quadraticBezierVertex(x, y + h, x, y + h - r, z, x + r, y + h);
	ofVertex(x, y + r, z);
	quadraticBezierVertex(x, y, x + r, y, z, x, y + r);
	ofEndShape();
}

void zukunftBlock::quadraticBezierVertex(float cpx, float cpy, float x, float y, float z, float prevX, float prevY) {
	float cp1x = prevX + 2.0 / 3.0*(cpx - prevX);
	float cp1y = prevY + 2.0 / 3.0*(cpy - prevY);
	float cp2x = cp1x + (x - prevX) / 3.0;
	float cp2y = cp1y + (y - prevY) / 3.0;

	// finally call cubic Bezier curve function  
	ofBezierVertex(cp1x, cp1y, z, cp2x, cp2y, z, x, y, z);

};
