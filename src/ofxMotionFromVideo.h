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
  const std::string getParameterGroupName();
  ofParameterGroup& getParameterGroup();

private:
  ofVideoPlayer video;
  PingPongFbo videoFbo;
  ofFbo opticalFlowFbo;
  OpticalFlowShader opticalFlowShader;
  bool doneFirstMotionRender { false };
};
