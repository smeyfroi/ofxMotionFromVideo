#pragma once

#include <memory>
#include <string>

#include "ofMain.h"
#include "ofxGui.h"
#include "ofxMotionFromVideo.h"

class ofApp : public ofBaseApp {
public:
  void setup() override;
  void update() override;
  void draw() override;
  void keyPressed(int key) override;
  void dragEvent(ofDragInfo dragInfo) override;

private:
  void activateCameraSource();
  void activateFileSource(const std::string& path);
  void setSource(std::shared_ptr<IFrameSource> source, const std::string& label);

  MotionFromVideo motionFromVideo;
  std::shared_ptr<IFrameSource> frameSourcePtr;

  ofxPanel gui;
  ofParameterGroup parameters;

  std::string sourceLabel = "none";
  std::string droppedFilePath;
};
