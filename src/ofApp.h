#pragma once

#include "ofMain.h"
#include "ofxOpenCv.h"
#include "ofxKinect.h"
#include "ofxGui.h"
#include "zukunftEvent.h"
#include "ofxRay.h"
#include "GuiApp.h"

// Windows users:
// You MUST install the libfreenect kinect drivers in order to be able to use
// ofxKinect. Plug in the kinect and point your Windows Device Manager to the
// driver folder in:
//
//     ofxKinect/libs/libfreenect/platform/windows/inf
//
// This should install the Kinect camera, motor, & audio drivers.
//
// You CANNOT use this driver and the OpenNI driver with the same device. You
// will have to manually update the kinect device to use the libfreenect drivers
// and/or uninstall/reinstall it in Device Manager.
//
// No way around the Windows driver dance, sorry.

// uncomment this to read from two kinects simultaneously
//#define USE_TWO_KINECTS

class ofApp : public ofBaseApp {
public:
	
	void setup();
	void update();
	void draw();
	void exit();
	
	void drawTable();

	void drawPointCloud();
	
	void keyPressed(int key);
	void mouseDragged(int x, int y, int button);
	void mousePressed(int x, int y, int button);
	void mouseReleased(int x, int y, int button);
	void mouseEntered(int x, int y);
	void mouseExited(int x, int y);
	void windowResized(int w, int h);
	void mouseScrolled(int x, int y, float scrollX, float scrollY);

	void drawFboTest(); // draws some test graphics into the two fbos

	
	ofxKinect kinect;
	
#ifdef USE_TWO_KINECTS
	ofxKinect kinect2;
#endif
	
	ofxCvGrayscaleImage colorImg;
	ofxCvColorImage kinectImg;

	ofxCvGrayscaleImage grayImage; // grayscale depth image
	ofxCvGrayscaleImage grayThreshNear; // the near thresholded image
	ofxCvGrayscaleImage grayThreshFar; // the far thresholded image
	
	ofxCvContourFinder contourFinder;

	ofxCvBlob blobFinder;
	
	bool bThreshWithOpenCV;
	bool bDrawPointCloud;
	
	int nearThreshold;
	int farThreshold;
	
	int angle;
	
	// used for viewing the point cloud
	ofEasyCam easyCam;


	//  proto header



	ofVec3f getScreenPoint(int x, int y);

	vector <zukunftBlock> blocks;
	ofCamera camera;

	int timelineL = 1200;

	float badgeSizeX = 100;
	float badgeSizeY = 60;

	bool islocked = false;
	int lockedElement;



	//8 bits red, 8 bits green, 8 bits blue, from 0 to 255 in 256 steps
	ofFbo rgbaFbo; // with alpha

				   //32 bits red, 32 bits green, 32 bits blue, from 0 to 1 in 'infinite' steps	
	ofFbo rgbaFboFloat; // with alpha

	int fadeAmnt;
	float* posx = 0;
	float* posy = 0;

	shared_ptr<GuiApp> gui;

	std::vector<cv::KeyPoint> keypoints;
	int test = 0;

	

};
