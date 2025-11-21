#include "ofApp.h"
#include <filesystem>

constexpr float FLUID_SIM_SCALE = 0.5;

//--------------------------------------------------------------
void ofApp::setup() {
  ofDisableArbTex(); // required for texture2D to work in GLSL, makes texture coords normalized
  ofEnableAlphaBlending();

  motionFromVideo.load("/Users/steve/Documents/music-source-material/belfast/trombone-trimmed.mov", true); // mute

  fluidSimulation.setup(ofGetWindowSize()*FLUID_SIM_SCALE);
  parameters.add(motionFromVideo.getParameterGroup());
  parameters.add(fluidSimulation.getParameterGroup());
  gui.setup(parameters);
  
  addTextureShader.load();
}

//--------------------------------------------------------------
void ofApp::update(){
  motionFromVideo.update();
  
  // Dump some of the video into the fluid values
  addTextureShader.render(fluidSimulation.getFlowValuesFbo(), motionFromVideo.getVideoFbo().getTexture(), 0.001);

  // Add some of the motion into the fluid velocities
  addTextureShader.render(fluidSimulation.getFlowVelocitiesFbo(), motionFromVideo.getMotionFbo().getTexture(), 0.01);
  
  fluidSimulation.update();
}

//--------------------------------------------------------------
void ofApp::draw(){
  ofSetWindowTitle(ofToString(ofGetFrameRate()));
  fluidSimulation.draw(0, 0, ofGetWindowWidth(), ofGetWindowHeight());
  ofPushMatrix();
  ofScale(ofGetWindowWidth(), ofGetWindowHeight());
  motionFromVideo.draw();
  ofPopMatrix();
  gui.draw();
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
