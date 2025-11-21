#include "ofApp.h"
#include <filesystem>
#include "ofxTimeMeasurements.h"

//--------------------------------------------------------------
void ofApp::setup() {
  ofDisableArbTex();
  ofSetFrameRate(30);

//  motionFromVideo.load(ofToDataPath("trimmed.mov"));
  motionFromVideo.initialiseCamera(0, { 1280, 720 });
  
  parameters.add(motionFromVideo.getParameterGroup());
  gui.setup(parameters);
  
  TIME_SAMPLE_SET_FRAMERATE(30);
}

//--------------------------------------------------------------
void ofApp::update(){
  TSGL_START("update");
  TS_START("update");
  motionFromVideo.update();
  TS_STOP("update");
  TSGL_STOP("update");
}

//--------------------------------------------------------------
void ofApp::draw(){
  TSGL_START("update");
  TS_START("update");

//  ofEnableBlendMode(OF_BLENDMODE_ALPHA);
//  ofSetColor(ofFloatColor { 1.0, 1.0, 1.0, 1.0 });
//  motionFromVideo.getVideoFbo().draw(0, 0, ofGetWindowWidth(), ofGetWindowHeight());
  ofEnableBlendMode(OF_BLENDMODE_ALPHA);
  ofSetColor(ofFloatColor { 1.0, 1.0, 1.0, 1.0 });
  motionFromVideo.getMotionFbo().draw(0, 0, ofGetWindowWidth(), ofGetWindowHeight());

  gui.draw();
  TS_STOP("update");
  TSGL_STOP("update");
}

//--------------------------------------------------------------
void ofApp::exit(){
}

//--------------------------------------------------------------
void ofApp::keyPressed(int key){
  motionFromVideo.keyPressed(key);
}

//--------------------------------------------------------------
void ofApp::keyReleased(int key){

}

//--------------------------------------------------------------
void ofApp::mouseMoved(int x, int y){

}

//--------------------------------------------------------------
void ofApp::mouseDragged(int x, int y, int button){

}

//--------------------------------------------------------------
void ofApp::mousePressed(int x, int y, int button){

}

//--------------------------------------------------------------
void ofApp::mouseReleased(int x, int y, int button){

}

//--------------------------------------------------------------
void ofApp::windowResized(int w, int h){

}

//--------------------------------------------------------------
void ofApp::gotMessage(ofMessage msg){

}

//--------------------------------------------------------------
void ofApp::dragEvent(ofDragInfo dragInfo){ 

}
