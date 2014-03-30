#include "testApp.h"
#include "ofxJSONElement.h"

using namespace ofxCv;

ofxFaceTracker::Gesture gestureIds[] = {
    ofxFaceTracker::MOUTH_WIDTH,
    ofxFaceTracker::MOUTH_HEIGHT,
    ofxFaceTracker::LEFT_EYEBROW_HEIGHT,
    ofxFaceTracker::RIGHT_EYEBROW_HEIGHT,
    ofxFaceTracker::LEFT_EYE_OPENNESS,
    ofxFaceTracker::RIGHT_EYE_OPENNESS,
    ofxFaceTracker::JAW_OPENNESS,
    ofxFaceTracker::NOSTRIL_FLARE
};

string gestureNames[] = {
    "mouthWidth",
    "mouthHeight",
    "leftEyebrowHeight",
    "rightEyebrowHeight",
    "leftEyeOpenness",
    "rightEyeOpenness",
    "jawOpenness",
    "nostrilFlare"
};

int gestureCount = 8;
void testApp::save(string filename){

    // export tracking data in all frames
    // ofVideoPlayer::getCurrentFrame() returns ofVideoPlayer::getTotalNumFrames() + 1
    int numFrames = video.getTotalNumFrames() + 1;
    for (int i=0; i<numFrames; i++) {
        video.setFrame(i);
        video.update();
        tracker.update(toCv(video));
        trackedImagePoints.push_back(tracker.getImageMesh());
        trackedObjectPoints.push_back(tracker.getObjectMesh());
        vector<float> curGesture;
        for(int i = 0; i < gestureCount; i++) {
            curGesture.push_back(tracker.getGesture(gestureIds[i]));
        }
        trackedGestures.push_back(curGesture);
        cout << "push_back tracker data: " << i << " / " << numFrames << endl;
    }

    // save tracking gestures, image points and object points as json data
    ofFile out(filename, ofFile::WriteOnly);
    out << "{\"results\": [";
    for(int i = 0; i < numFrames; i++) {
        out << "{";
        //gestures
        for(int j = 0; j < gestureCount; j++) {
            out << "\""+gestureNames[j]+"\"" << ":" << trackedGestures[i][j] << "," << endl;
        }

        //imagepoints
        out << "\"imagepoints\": [";
        for(int j = 0; j < trackedImagePoints[i].getNumVertices(); j++) {
            if (j == trackedImagePoints[i].getNumVertices() - 1) {
                out << "[" << ofVec2f(trackedImagePoints[i].getVertex(j)) << "]" << endl;
            }else{
                out << "[" << ofVec2f(trackedImagePoints[i].getVertex(j)) << "]," << endl;
            }
        }
        out << "],";

        //objectpoints
        out <<"\"objectpoints\": [";
        for(int j = 0; j < trackedObjectPoints[i].getNumVertices(); j++) {
            if (j == trackedObjectPoints[i].getNumVertices() - 1) {
                out << "[" << ofVec2f(trackedObjectPoints[i].getVertex(j)) << "]" << endl;
            }else{
                out << "[" << ofVec2f(trackedObjectPoints[i].getVertex(j)) << "]," << endl;
            }
        }
        out << "]"<<endl;

        if (i == numFrames - 1) {
            out << "}"<<endl;

        }else{
            out << "},"<<endl;
        }
    }
    out << "]}";

}
void testApp::load(string filename){
    ofxJSONElement element;
    element.open(filename);

    for (int i=0; i<element["results"].size(); i++) {
        //gesture
        vector<float>val_list;
        for (int j=0; j<gestureCount; j++) {
            float val = element["results"][i][gestureNames[j]].asFloat();
            val_list.push_back(val);
        }
        recordedGestures.push_back(val_list);

        //image points
        ofMesh imagemesh;
        for (int j=0; j<element["results"][i]["imagepoints"].size(); j++) {
            float px = element["results"][i]["imagepoints"][j][0].asFloat();
            float py = element["results"][i]["imagepoints"][j][1].asFloat();
            imagemesh.addVertex(ofVec3f(px, py, 0));
        }
        recordedImagePoints.push_back(imagemesh);

        //object points
        ofMesh objectmesh;
        for (int j=0; j<element["results"][i]["objectpoints"].size(); j++) {
            float px = element["results"][i]["objectpoints"][j][0].asFloat();
            float py = element["results"][i]["objectpoints"][j][1].asFloat();
            objectmesh.addVertex(ofVec3f(px, py, 0));
        }
        recordedObjectPoints.push_back(objectmesh);
    }
}

int currentFrame = 0;

void testApp::setup() {
    ofSetFrameRate(30);
    //h264 codec is not recommended
    video.loadMovie("video.mov");
	tracker.setup();
    tracker.setRescale(.25);
    tracker.setIterations(100);
    tracker.setClamp(10);
    tracker.setTolerance(.5);
    tracker.setAttempts(4);
    save("data.json");
    load("data.json");
    video.play();
}

void testApp::update() {
    video.update();
}

void testApp::draw() {
    float scale = ofGetWidth() / video.getWidth();
    ofScale(scale, scale);
    video.draw(0, 0);
    for (auto v : recordedImagePoints[video.getCurrentFrame()].getVertices())
         ofCircle(v, 5);
}

void testApp::keyPressed(int key) {
    if(key == 's') {
        save("data.json");
    }
    if(key == 'l') {
        load("data.json");
    }
}