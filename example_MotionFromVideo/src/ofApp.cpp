#include "ofApp.h"
#include <filesystem>

//--------------------------------------------------------------
void ofApp::setup() {
  ofDisableArbTex(); // required for texture2D to work in GLSL, makes texture coords normalized


//  const std::filesystem::path rootSourceMaterialPath { "/Users/steve/Documents/music-source-material" };
//  
//  audioAnalysisClientPtr = std::make_shared<ofxAudioAnalysisClient::LocalGistClient>(rootSourceMaterialPath/"20250207-violin-busy-trill.wav");
  
  motionFromVideo.load(ofToDataPath("trimmed.mov"));
  
  parameters.add(motionFromVideo.getParameterGroup());
  gui.setup(parameters);
//  gui.getGroup(motionFromVideo.getParameterGroupName()).minimize();
  
//  multiplyColorShader.load();
  logisticFnShader.load();
}

//--------------------------------------------------------------
void ofApp::update(){
  motionFromVideo.update();
}

//--------------------------------------------------------------
void ofApp::draw(){
  logisticFnShader.render(motionFromVideo.getMotionFbo(), glm::vec4 { 1.0, 1.0, 0.0, 0.0 });
//  multiplyColorShader.render(motionFromVideo.getMotionFbo(), glm::vec4 { 0.5, 0.5, 1.0, 1.0 });
//  motionFromVideo.drawMotion();
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
