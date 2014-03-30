#pragma once

#include "ofMain.h"
#include "ofxCv.h"

#include "ofxFaceTracker.h"

class testApp : public ofBaseApp {
public:
	void setup();
	void update();
	void draw();
    void keyPressed(int key);
    void save(string filename);
    void load(string filename);
	ofVideoPlayer video;
	ofxFaceTracker tracker;
    vector<ofMesh> trackedImagePoints;
    vector<ofMesh> trackedObjectPoints;
    vector<vector<float> > trackedGestures;

    vector<ofMesh> recordedImagePoints;
    vector<ofMesh> recordedObjectPoints;
    vector<vector<float> > recordedGestures;

};
