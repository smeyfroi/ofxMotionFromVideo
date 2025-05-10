#pragma once

#include "ofMain.h"
#include "PingPongFbo.h"
#include "OpticalFlowShader.h"
//#include "AddTextureShader.h"
//#include "FluidSimulation.h"
//#include "FadeShader.h"

class MotionFromVideo {

public:
  MotionFromVideo();
  ~MotionFromVideo();
  void load(const std::string& path, bool mute = true);
  void update();
  bool keyPressed(int key);
  void drawVideo();
  const ofFbo& getVideoFbo() { return videoFbo.getSource(); };
  void drawMotion();
  const ofFbo& getMotionFbo() const { return opticalFlowFbo; };
  const ofFloatPixels& getMotionPixels() const { return opticalFlowPixels; };
  // { x, y, dx, dy }
  std::optional<glm::vec4> trySampleMotion() const;
  const std::string getParameterGroupName();
  ofParameterGroup& getParameterGroup();

private:
  ofVideoPlayer video;
  PingPongFbo videoFbo;
  ofFbo opticalFlowFbo;
  OpticalFlowShader opticalFlowShader;
  bool doneFirstMotionRender { false };
  
  ofFloatPixels opticalFlowPixels;
  
  ofParameterGroup parameters;
  ofParameter<float> xFlowThresholdNeg {"xFlowThresholdNeg", -0.1, -0.5, 0.0};
  ofParameter<float> xFlowThresholdPos {"xFlowThresholdPos", 0.1, 0.0, 0.5};
  ofParameter<float> yFlowThresholdNeg {"yFlowThresholdNeg", -0.1, -0.5, 0.0};
  ofParameter<float> yFlowThresholdPos {"yFlowThresholdPos", 0.1, 0.0, 0.5};

};
