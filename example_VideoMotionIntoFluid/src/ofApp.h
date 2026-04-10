#pragma once

#include <memory>
#include <string>

#include "AddTextureShader.h"
#include "FluidSimulation.h"
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
  void setUpstreamSource(std::shared_ptr<IFrameSource> source, const std::string& label);

  std::shared_ptr<IFrameSource> upstreamSourcePtr;
  std::shared_ptr<ExternalFrameSource> externalFrameSourcePtr;
  MotionFromVideo motionFromVideo;

  AddTextureShader addTextureShader;
  FluidSimulation fluidSimulation;

  ofxPanel gui;
  ofParameterGroup parameters;

  std::string sourceLabel = "camera";
  std::string droppedFilePath;
};
