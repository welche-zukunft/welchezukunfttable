#ifndef _ZUKUNFTEVENT
#define _ZUKUNFTEVENT
#include "ofMain.h"
class zukunftBlock {

public:

	void setup(ofPoint p, float x, float y, string name);
	void update();
	void draw();
	void roundedRect(float x, float y, float z, float w, float h, float r);
	void quadraticBezierVertex(float cpx, float cpy, float x, float y, float z, float prevX, float prevY);
	ofPoint position;
	float sizex;
	float sizey;
	string content;
	ofColor color;
	ofImage image;
	ofPlanePrimitive plane;
	ofPoint offset = { 0,0,40 };
	int imageAreaTop = -50;
	int imageAreaBottom = -100;
	int overUnder;
	ofFbo texture;

	zukunftBlock();
private:

};




#endif // !ZUKUNFTEVENT

