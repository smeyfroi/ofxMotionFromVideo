#include "ofApp.h"
#include <filesystem>

constexpr float SCALE = 1.0;

//--------------------------------------------------------------
void ofApp::setup() {
  ofDisableArbTex(); // required for texture2D to work in GLSL, makes texture coords normalized
  ofEnableAlphaBlending();

  motionFromVideo.load(ofToDataPath("trimmed.mov"));

  fluidSimulation.setup(ofGetWindowSize()*SCALE);
  parameters.add(motionFromVideo.getParameterGroup());
  parameters.add(fluidSimulation.getParameterGroup());
  gui.setup(parameters);
  
  addTextureShader.load();
}

//--------------------------------------------------------------
void ofApp::update(){
  motionFromVideo.update();
  
  // Dump some of the motion into the fluid values
  fluidSimulation.getFlowValuesFbo().getTarget().begin();
  addTextureShader.render(fluidSimulation.getFlowValuesFbo().getSource(), motionFromVideo.getMotionFbo(), 0.03);
  fluidSimulation.getFlowValuesFbo().getTarget().end();
  fluidSimulation.getFlowValuesFbo().swap();

  // Add the video motion into the fluid velocities
  fluidSimulation.getFlowVelocitiesFbo().getTarget().begin();
  addTextureShader.render(fluidSimulation.getFlowVelocitiesFbo().getSource(), motionFromVideo.getMotionFbo(), 0.005);
  fluidSimulation.getFlowVelocitiesFbo().getTarget().end();
  fluidSimulation.getFlowVelocitiesFbo().swap();
  
  fluidSimulation.update();
}

//--------------------------------------------------------------
void ofApp::draw(){
  ofSetWindowTitle(ofToString(ofGetFrameRate()));
  fluidSimulation.draw(0, 0, ofGetWindowWidth(), ofGetWindowHeight());
  gui.draw();
}

//--------------------------------------------------------------
void ofApp::exit(){
}

//--------------------------------------------------------------
void ofApp::keyPressed(int key){
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
