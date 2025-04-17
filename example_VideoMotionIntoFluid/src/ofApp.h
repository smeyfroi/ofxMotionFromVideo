#pragma once

#include "ofMain.h"
#include "ofxMotionFromVideo.h"
#include "ofxGui.h"
#include "AddTextureShader.h"
#include "FluidSimulation.h"

class ofApp: public ofBaseApp{
public:
  void setup();
  void update();
  void draw();
  void exit();
  
  void keyPressed(int key);
  void keyReleased(int key);
  void mouseMoved(int x, int y);
  void mouseDragged(int x, int y, int button);
  void mousePressed(int x, int y, int button);
  void mouseReleased(int x, int y, int button);
  void windowResized(int w, int h);
  void dragEvent(ofDragInfo dragInfo);
  void gotMessage(ofMessage msg);
  
private:
  MotionFromVideo motionFromVideo;
  
  AddTextureShader addTextureShader;
  
  FluidSimulation fluidSimulation;
  
  bool guiVisible { false };
  ofxPanel gui;
  ofParameterGroup parameters;

};
