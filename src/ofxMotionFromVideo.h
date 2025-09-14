#pragma once

#include "ofMain.h"
#include "PingPongFbo.h"
#include "OpticalFlowShader.h"

class MotionFromVideo {

public:
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
  
  glm::vec2 getSize() const { return size; };
  const std::string getParameterGroupName() const;
  ofParameterGroup& getParameterGroup();
  bool isReady() { return videoFbo.getSource().isAllocated() && startupFrame == 0; };

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
  
  ofParameterGroup parameters;

  bool videoVisible { false };
  bool motionVisible { false };
};
