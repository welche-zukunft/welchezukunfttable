#include "ofApp.h"

/*
    If you are struggling to get the device to connect ( especially Windows Users )
    please look at the ReadMe: in addons/ofxKinect/README.md
*/

//--------------------------------------------------------------
void ofApp::setup() {
	ofSetLogLevel(OF_LOG_VERBOSE);
	
	// enable depth->video image calibration
	kinect.setRegistration(true);
    
	//kinect.init();
	kinect.init(true); // shows infrared instead of RGB video image
	//kinect.init(false, false); // disable video image (faster fps)
	
	kinect.open();		// opens first available kinect
	//kinect.open(1);	// open a kinect by id, starting with 0 (sorted by serial # lexicographically))
	//kinect.open("A00362A08602047A");	// open a kinect using it's unique serial #
	
	// print the intrinsic IR sensor values
	if(kinect.isConnected()) {
		ofLogNotice() << "sensor-emitter dist: " << kinect.getSensorEmitterDistance() << "cm";
		ofLogNotice() << "sensor-camera dist:  " << kinect.getSensorCameraDistance() << "cm";
		ofLogNotice() << "zero plane pixel size: " << kinect.getZeroPlanePixelSize() << "mm";
		ofLogNotice() << "zero plane dist: " << kinect.getZeroPlaneDistance() << "mm";
	}
	
#ifdef USE_TWO_KINECTS
	kinect2.init();
	kinect2.open();
#endif
	
	colorImg.allocate(kinect.width, kinect.height);
	grayImage.allocate(kinect.width, kinect.height);
	grayThreshNear.allocate(kinect.width, kinect.height);
	grayThreshFar.allocate(kinect.width, kinect.height);
	
	nearThreshold = 230;
	farThreshold = 18;
	bThreshWithOpenCV = true;
	
	ofSetFrameRate(60);
	
	// zero the tilt on startup
	angle = 0;
	kinect.setCameraTiltAngle(angle);
	
	// start from the front
	bDrawPointCloud = false;



	ofBackground(0, 0, 0);

	//allocate our fbos. 
	//providing the dimensions and the format for the,
	rgbaFbo.allocate(400, 300, GL_RGBA); // with alpha, 8 bits red, 8 bits green, 8 bits blue, 8 bits alpha, from 0 to 255 in 256 steps	

#ifdef TARGET_OPENGLES
	rgbaFboFloat.allocate(400, 400, GL_RGBA); // with alpha, 32 bits red, 32 bits green, 32 bits blue, 32 bits alpha, from 0 to 1 in 'infinite' steps
	ofLogWarning("ofApp") << "GL_RGBA32F_ARB is not available for OPENGLES.  Using RGBA.";
#else
	rgbaFboFloat.allocate(400, 300, GL_RGBA32F_ARB); // with alpha, 32 bits red, 32 bits green, 32 bits blue, 32 bits alpha, from 0 to 1 in 'infinite' steps
#endif

													 // we can also define the fbo with ofFbo::Settings.
													 // this allows us so set more advanced options the width (400), the height (200) and the internal format like this
													 /*
													 ofFbo::Settings s;
													 s.width			= 400;
													 s.height			= 200;
													 s.internalformat   = GL_RGBA;
													 s.useDepth			= true;
													 // and assigning this values to the fbo like this:
													 rgbFbo.allocate(s);
													 */


													 // we have to clear all the fbos so that we don't see any artefacts
													 // the clearing color does not matter here, as the alpha value is 0, that means the fbo is cleared from all colors
													 // whenever we want to draw/update something inside the fbo, we have to write that inbetween fbo.begin() and fbo.end()

	rgbaFbo.begin();
	ofClear(255, 255, 255, 0);
	rgbaFbo.end();

	//// rgbaFboFloat.begin();
	//// ofClear(255, 255, 255, 0);
	//// rgbaFboFloat.end();


}

//--------------------------------------------------------------
void ofApp::update() {

	
	cv::SimpleBlobDetector::Params params;
	params.minThreshold = 10;
	params.filterByArea = true;
	params.minArea = 20;
	params.maxArea = 100;
	params.filterByInertia = false;
	params.filterByColor = false;


	cv::SimpleBlobDetector detector(params);

	ofBackground(100, 100, 100);
	kinect.update();
	// there is a new frame and we are connected
	if(kinect.isFrameNew()) {
		
		// load grayscale image from the kinect source
		grayImage.setFromPixels(kinect.getPixels());
		// we do two thresholds - one for the far plane and one for the near plane
		// we then do a cvAnd to get the pixels which are a union of the two thresholds
		if(bThreshWithOpenCV) {
			grayThreshNear = grayImage;
			grayThreshFar = grayImage;
			grayThreshNear.threshold(nearThreshold, true);
			grayThreshFar.threshold(farThreshold);
			cvAnd(grayThreshNear.getCvImage(), grayThreshFar.getCvImage(), grayImage.getCvImage(), NULL);
		} else {
			
			// or we do it ourselves - show people how they can work with the pixels
			ofPixels & pix = grayImage.getPixels();
			int numPixels = pix.size();
			for(int i = 0; i < numPixels; i++) {
				if(pix[i] < nearThreshold && pix[i] > farThreshold) {
					pix[i] = 255;
				} else {
					pix[i] = 0;
				}
			}
		}
		
		// update the cv images
		grayImage.flagImageChanged();
		kinectImg = kinect.getPixels();
		//cv::Mat im = grayImage.getCvImage();
		
		
		
		cv::Mat im = kinectImg.getCvImage();
		detector.detect(im, keypoints);
		test = keypoints.size();
		cv::Mat imWith;
		drawKeypoints(im, keypoints, imWith, cv::Scalar(0, 0, 255), cv::DrawMatchesFlags::DRAW_RICH_KEYPOINTS);


		colorImg.setFromPixels((unsigned char *)IplImage(imWith).imageData, imWith.size().width, imWith.size().height); // , OF_IMAGE_COLOR);

		//*(grayImage.getCvImage()) = imWith;
		
		
		// find contours which are between the size of 20 pixels and 1/3 the w*h pixels.
		// also, find holes is set to true so we will get interior contours as well....
		/*contourFinder.findContours(grayImage, 10, (kinect.width*kinect.height)/2, 20, false);  
		int numBlobs = contourFinder.blobs.size();
		for (int i = 0; i < numBlobs; ++i) {
			ofxCvBlob & blob = contourFinder.blobs[i];
			std::cout << "X Value: " << blob.centroid.x << endl;
			std::cout << "Y Value: " << blob.centroid.y << endl;
			*posx = blob.centroid.x;
			*posy = blob.centroid.y;

		}*/

		int numBlobs = keypoints.size();
		for (int i = 0; i < numBlobs; ++i) {
			// ofxCvBlob & blob = keypoints[i];
			//keypoints[i].pt.x
			std::cout << "X Value: " << keypoints[i].pt.x << endl;
			std::cout << "Y Value: " << keypoints[i].pt.y << endl;
			*posx = keypoints[i].pt.x;
			*posy = keypoints[i].pt.y;

		}
	}
	
#ifdef USE_TWO_KINECTS
	kinect2.update();
#endif

/*
	ofEnableAlphaBlending();

	//lets draw some graphics into our two fbos
	rgbaFbo.begin();
	drawFboTest();
	rgbaFbo.end();
	*/
	//// rgbaFboFloat.begin();
	//// drawFboTest();
	//// rgbaFboFloat.end();
}

//--------------------------------------------------------------
void ofApp::draw() {

	ofSetColor(255, 255, 255);
	//rgbaFbo.draw(420, 320, 400, 300);

	//// ofDrawBitmapString("non floating point FBO", ofPoint(10, 20));
	ofDrawBitmapString("floating point FBO", ofPoint(420, 20));

	string alphaInfo = "Current alpha fade amnt = " + ofToString(fadeAmnt);
	alphaInfo += "\nHold '1' to set alpha fade to 1";
	alphaInfo += "\nHold '2' to set alpha fade to 5";
	alphaInfo += "\nHold '3' to set alpha fade to 15";
	alphaInfo += "\nHold 'c' to clear the fbo each frame\n\nMove mouse to draw with a circle";

	ofDrawBitmapString(alphaInfo, ofPoint(10, 430));




	
	ofSetColor(255, 255, 255);
	
	if(bDrawPointCloud) {
		easyCam.begin();
		drawPointCloud();
		easyCam.end();
	} else {
		// draw from the live kinect
		kinect.draw(10, 10, 400, 300);
		kinect.draw(420, 10, 400, 300);
		
		grayImage.draw(10, 320, 400, 300);
		colorImg.draw(840, 10, 400, 300);

		contourFinder.draw(10, 320, 400, 300);

		ofDrawCircle(ofMap(*posx, 0, 640, 420, 820), ofMap(*posy, 0, 480, 320, 620), 20);


#ifdef USE_TWO_KINECTS
		kinect2.draw(420, 320, 400, 300);
#endif
	}
	
	// draw instructions
	ofSetColor(255, 255, 255);
	stringstream reportStream;
        
    if(kinect.hasAccelControl()) {
        reportStream << "accel is: " << ofToString(kinect.getMksAccel().x, 2) << " / "
        << ofToString(kinect.getMksAccel().y, 2) << " / "
        << ofToString(kinect.getMksAccel().z, 2) << endl;
    } else {
        reportStream << "Note: this is a newer Xbox Kinect or Kinect For Windows device," << endl
		<< "motor / led / accel controls are not currently supported" << endl << endl;
    }
    
	reportStream << "press p to switch between images and point cloud, rotate the point cloud with the mouse" << endl
		<< "using opencv threshold = " << bThreshWithOpenCV << " (press spacebar)" << endl
		<< "set near threshold " << nearThreshold << " (press: + -)" << endl
		<< "set far threshold " << farThreshold << " (press: < >) num blobs found " << contourFinder.nBlobs
		<< ", fps: " << ofGetFrameRate() << endl
		<< "press c to close the connection and o to open it again, connection is: " << kinect.isConnected()
		<< "X POS : " << *posx << "    Y POS : " << *posy << endl
		<< test << endl;

    if(kinect.hasCamTiltControl()) {
    	reportStream << "press UP and DOWN to change the tilt angle: " << angle << " degrees" << endl
        << "press 1-5 & 0 to change the led mode" << endl;
    }
    
	ofDrawBitmapString(reportStream.str(), 20, 652);
    
}



//--------------------------------------------------------------
void ofApp::drawTable() {


}




void ofApp::drawPointCloud() {
	int w = 640;
	int h = 480;
	ofMesh mesh;
	mesh.setMode(OF_PRIMITIVE_POINTS);
	int step = 2;
	for(int y = 0; y < h; y += step) {
		for(int x = 0; x < w; x += step) {
			if(kinect.getDistanceAt(x, y) > 0) {
				mesh.addColor(kinect.getColorAt(x,y));
				mesh.addVertex(kinect.getWorldCoordinateAt(x, y));
			}
		}
	}
	glPointSize(3);
	ofPushMatrix();
	// the projected points are 'upside down' and 'backwards' 
	ofScale(1, -1, -1);
	ofTranslate(0, 0, -1000); // center the points a bit
	ofEnableDepthTest();
	mesh.drawVertices();
	ofDisableDepthTest();
	ofPopMatrix();
}

//--------------------------------------------------------------
void ofApp::exit() {
	kinect.setCameraTiltAngle(0); // zero the tilt on exit
	kinect.close();
	
#ifdef USE_TWO_KINECTS
	kinect2.close();
#endif
}

//--------------------------------------------------------------
void ofApp::keyPressed (int key) {
	switch (key) {
		case ' ':
			bThreshWithOpenCV = !bThreshWithOpenCV;
			break;
			
		case'p':
			bDrawPointCloud = !bDrawPointCloud;
			break;
			
		case '>':
		case '.':
			farThreshold ++;
			if (farThreshold > 255) farThreshold = 255;
			break;
			
		case '<':
		case ',':
			farThreshold --;
			if (farThreshold < 0) farThreshold = 0;
			break;
			
		case '+':
		case '=':
			nearThreshold ++;
			if (nearThreshold > 255) nearThreshold = 255;
			break;
			
		case '-':
			nearThreshold --;
			if (nearThreshold < 0) nearThreshold = 0;
			break;
			
		case 'w':
			kinect.enableDepthNearValueWhite(!kinect.isDepthNearValueWhite());
			break;
			
		case 'o':
			kinect.setCameraTiltAngle(angle); // go back to prev tilt
			kinect.open();
			break;
			
		case 'c':
			kinect.setCameraTiltAngle(0); // zero the tilt
			kinect.close();
			break;
			
		case '1':
			kinect.setLed(ofxKinect::LED_GREEN);
			break;
			
		case '2':
			kinect.setLed(ofxKinect::LED_YELLOW);
			break;
			
		case '3':
			kinect.setLed(ofxKinect::LED_RED);
			break;
			
		case '4':
			kinect.setLed(ofxKinect::LED_BLINK_GREEN);
			break;
			
		case '5':
			kinect.setLed(ofxKinect::LED_BLINK_YELLOW_RED);
			break;
			
		case '0':
			kinect.setLed(ofxKinect::LED_OFF);
			break;
			
		case OF_KEY_UP:
			angle++;
			if(angle>30) angle=30;
			kinect.setCameraTiltAngle(angle);
			break;
			
		case OF_KEY_DOWN:
			angle--;
			if(angle<-30) angle=-30;
			kinect.setCameraTiltAngle(angle);
			break;
	}
}




//--------------------------------------------------------------
void ofApp::mouseDragged(int x, int y, int button) {
	//cout << button << x << y << endl;
}

void ofApp::mouseScrolled(int x, int y, float scrollX, float scrollY) {
	if (scrollY == -1) {
		ofVec3f pos = camera.getPosition();
		if (pos.z <= 340) {
			pos.z += 10.;
			camera.setPosition(pos);
		}
	}
	if (scrollY == 1) {
		ofVec3f pos = camera.getPosition();
		if (pos.z >= 40) {
			pos.z -= 10.;
			camera.setPosition(pos);
			//cout << camera.getGlobalPosition() << " - " << camera.getLookAtDir() << '\n';
		}
	}
}
//--------------------------------------------------------------
void ofApp::mousePressed(int x, int y, int button) {
	cout << button << endl;
	if (button == 0 && islocked == false) {

		zukunftBlock tempblock;
		string cont = to_string(blocks.size());
		tempblock.setup(getScreenPoint(x, y), badgeSizeX, badgeSizeY, cont);
		//ofLog() << getScreenPoint(x, y);
		blocks.push_back(tempblock);
	}
	if (button == 2 && islocked == false) {
		ofVec3f pointOnPlane2 = getScreenPoint(x, y);
		int n = blocks.size();
		for (int i = 0; i < n; i++) {
			ofVec3f curpos = blocks[i].position;
			if (pointOnPlane2.x >= curpos.x && pointOnPlane2.x <= curpos.x + badgeSizeX && pointOnPlane2.y > curpos.y && pointOnPlane2.y < curpos.y + badgeSizeY) {
				ofLog() << "value " << i;
				islocked = true;
				lockedElement = i;
			}
		}
	}

}

//--------------------------------------------------------------
void ofApp::mouseReleased(int x, int y, int button) {
	if (button == 2 && islocked == true) {
		blocks[lockedElement].position = getScreenPoint(x, y);
		islocked = false;
		lockedElement = 0;
	}
}

//--------------------------------------------------------------
void ofApp::mouseEntered(int x, int y) {

}

//--------------------------------------------------------------
void ofApp::mouseExited(int x, int y) {

}

//--------------------------------------------------------------
void ofApp::windowResized(int w, int h) {

}

//--------------------------------------------------------------

ofVec3f ofApp::getScreenPoint(int x, int y) {
	ofVec3f screenToWorld = camera.screenToWorld(ofVec3f(x, y, 0.0));
	ofxRay::Ray ray(camera.getPosition(), screenToWorld - camera.getPosition());
	ofxRay::Plane plane(ofVec3f(0, 0, 0), ofVec3f(0, 0, 1));
	ofVec3f pointOnPlane;
	plane.intersect(ray, pointOnPlane);
	return pointOnPlane;

}




void ofApp::drawFboTest() {
	//we clear the fbo if c is pressed. 
	//this completely clears the buffer so you won't see any trails
	if (ofGetKeyPressed('c')) {
		ofClear(255, 255, 255, 0);
	}

	//some different alpha values for fading the fbo
	//the lower the number, the longer the trails will take to fade away.
	fadeAmnt = 40;
	if (ofGetKeyPressed('1')) {
		fadeAmnt = 1;
	}
	else if (ofGetKeyPressed('2')) {
		fadeAmnt = 5;
	}
	else if (ofGetKeyPressed('3')) {
		fadeAmnt = 15;
	}

	//1 - Fade Fbo

	//this is where we fade the fbo
	//by drawing a rectangle the size of the fbo with a small alpha value, we can slowly fade the current contents of the fbo. 
	ofFill();
	ofSetColor(255, 255, 255, fadeAmnt);
	ofDrawRectangle(0, 0, 400, 400);

	//2 - Draw graphics

	ofNoFill();
	ofSetColor(255, 255, 255);

	//we draw a cube in the center of the fbo and rotate it based on time
	ofPushMatrix();
	ofTranslate(200, 200, 0);
	ofDrawBox(0, 0, 0, 100);
	ofPopMatrix();

	//also draw based on our mouse position
	ofFill();
	ofDrawCircle(ofGetMouseX() % 410, ofGetMouseY(), 8);

	//we move a line across the screen based on the time
	//the %400 makes the number stay in the 0-400 range. 
	int shiftX = (ofGetElapsedTimeMillis() / 8) % 400;

	ofDrawRectangle(shiftX, rgbaFbo.getHeight() - 30, 3, 30);

}
