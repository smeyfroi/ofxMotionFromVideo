#pragma once

#include "ofMain.h"
#include "PingPongFbo.h"
#include "OpticalFlowShader.h"

class MotionFromVideo {

public:
  MotionFromVideo();
  ~MotionFromVideo();
  void initialiseCamera(int deviceID, glm::vec2 size);
  void load(const std::string& path, bool mute = true);
  void update();
  bool keyPressed(int key);
  void draw();
  void drawVideo();
  const ofFbo& getVideoFbo() { return videoFbo.getSource(); };
  void drawMotion();
  const ofFbo& getMotionFbo() const { return opticalFlowFbo; };
  const ofFloatPixels& getMotionPixels() const { return opticalFlowPixels; };
  // { x, y, dx, dy }
  std::optional<glm::vec4> trySampleMotion();
  glm::vec2 getSize() const { return size; };
  const std::string getParameterGroupName() const;
  ofParameterGroup& getParameterGroup();
  bool isReady() { return videoFbo.getSource().isAllocated() &&  startupFrame == 0; };

private:
  void initialiseFbos(glm::vec2 size);

  bool isGrabbing;
  ofVideoGrabber videoGrabber;
  ofVideoPlayer videoPlayer;

  glm::vec2 size;
  PingPongFbo videoFbo;
  ofFbo opticalFlowFbo;
  OpticalFlowShader opticalFlowShader;
  int startupFrame { -30 }; // ignore the first few frames
  
  ofFloatPixels opticalFlowPixels;
  
  ofParameterGroup parameters;
  ofParameter<float> xFlowThresholdNeg {"xFlowThresholdNeg", -0.1, -0.5, 0.0};
  ofParameter<float> xFlowThresholdPos {"xFlowThresholdPos", 0.1, 0.0, 0.5};
  ofParameter<float> yFlowThresholdNeg {"yFlowThresholdNeg", -0.1, -0.5, 0.0};
  ofParameter<float> yFlowThresholdPos {"yFlowThresholdPos", 0.1, 0.0, 0.5};
  
  bool videoVisible;
  bool motionVisible;
};
