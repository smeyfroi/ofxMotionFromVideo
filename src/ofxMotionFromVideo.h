#pragma once

#include <optional>
#include <string>

#include "ofMain.h"
#include "PingPongFbo.h"
#include "OpticalFlowShader.h"

class MotionFromVideo {

public:
  ~MotionFromVideo();
  void initialiseCamera(int deviceID, glm::vec2 size);
  void load(const std::string& path, bool mute = true);
  void setPositionSeconds(int seconds);
  void stop();

  // External frame source (Synth-owned camera/file stream).
  // When set, MotionFromVideo will not own/advance any grabber/player.
  void setExternalFrames(const ofFbo* currentFrameFbo, const ofFbo* previousFrameFbo, bool hasNewFrame);

  void update();
  bool keyPressed(int key);
  void draw();
  void drawVideo();
  const ofFbo& getVideoFbo() const { return videoFbo.getSource(); };
  void drawMotion();
  const ofFbo& getMotionFbo() const { return opticalFlowFbo; };
  
  // Returns { x, y, dx, dy } where x,y are normalized [0..1] and dx,dy are scaled velocities.
  std::optional<glm::vec4> trySampleMotion();

  // Disables CPU readback when point sampling isn't used.
  void setCpuSamplingEnabled(bool enabled) { cpuSamplingEnabled = enabled; }
  bool isCpuSamplingEnabled() const { return cpuSamplingEnabled; }

  glm::vec2 getSize() const { return size; }
  const std::string getParameterGroupName() const;
  ofParameterGroup& getParameterGroup();
  bool isReady() const {
    if (useExternalFrames) {
      return externalCurrentFrameFbo && externalCurrentFrameFbo->isAllocated() && startupFrame == 0;
    }
    return videoFbo.getSource().isAllocated() && startupFrame == 0;
  };

  bool isVideoVisible() const { return videoVisible; }
  bool isMotionVisible() const { return motionVisible; }
  void setVideoVisible(bool visible) { videoVisible = visible; }
  void setMotionVisible(bool visible) { motionVisible = visible; }

private:
  void initialiseFbos(glm::vec2 size);

  bool isGrabbing;
  ofVideoGrabber videoGrabber;
  ofVideoPlayer videoPlayer;

  glm::vec2 size;
  PingPongFbo videoFbo;
  ofFbo opticalFlowFbo;
  OpticalFlowShader opticalFlowShader;
  bool opticalFlowShaderLoaded { false };
  int startupFrame { -30 }; // ignore the first few frames

  // External stream frames (if set)
  bool useExternalFrames { false };
  const ofFbo* externalCurrentFrameFbo { nullptr };
  const ofFbo* externalPreviousFrameFbo { nullptr };
  bool externalHasNewFrame { false };
  
  // Only allocated/updated when CPU sampling is enabled.
  ofFloatPixels opticalFlowPixels;
  bool cpuSamplingEnabled { false };

  ofParameterGroup parameters;

  // Gate for accepting sampled flow vectors (in flow-texture units, not pixel-scaled).
  // This ends up being venue/camera dependent; allow a wide range.
  ofParameter<float> minSpeedMagnitude {"MinSpeedMagnitude", 0.40f, 0.0f, 1.0f};

  bool videoVisible { false };
  bool motionVisible { false };
};
