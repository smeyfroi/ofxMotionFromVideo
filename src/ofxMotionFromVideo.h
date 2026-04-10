#pragma once

#include <memory>
#include <optional>
#include <string>

#include "CameraFrameSource.hpp"
#include "ExternalFrameSource.hpp"
#include "IFrameSource.hpp"
#include "OpticalFlowShader.h"
#include "VideoFileFrameSource.hpp"
#include "ofMain.h"

class MotionFromVideo {
public:
  ~MotionFromVideo();

  void setFrameSource(std::shared_ptr<IFrameSource> frameSource);
  const std::shared_ptr<IFrameSource>& getFrameSource() const { return frameSourcePtr; }

  void initialiseCamera(int deviceId, glm::vec2 size);
  void load(const std::string& path, bool mute = true);
  void setPositionSeconds(int seconds);
  void stop();

  void setExternalFrames(const ofFbo* currentFrameFbo,
                         const ofFbo* previousFrameFbo,
                         bool hasNewFrame,
                         bool mirrored = false);

  void update();
  bool keyPressed(int key);
  void draw();
  void drawVideo();
  const ofFbo& getVideoFbo() const;
  void drawMotion();
  const ofFbo& getMotionFbo() const { return opticalFlowFbo; }

  std::optional<glm::vec4> trySampleMotion();

  void setCpuSamplingEnabled(bool enabled) { cpuSamplingEnabled = enabled; }
  bool isCpuSamplingEnabled() const { return cpuSamplingEnabled; }

  glm::vec2 getSize() const { return size; }
  const std::string getParameterGroupName() const;
  ofParameterGroup& getParameterGroup();
  bool isReady() const;

  bool isVideoVisible() const { return videoVisible; }
  bool isMotionVisible() const { return motionVisible; }
  void setVideoVisible(bool visible) { videoVisible = visible; }
  void setMotionVisible(bool visible) { motionVisible = visible; }

private:
  void initialiseFbos(glm::vec2 size_);

  std::shared_ptr<IFrameSource> frameSourcePtr;
  std::shared_ptr<ExternalFrameSource> externalFrameSourcePtr;
  ofFbo emptyFrameFbo;

  glm::vec2 size { 0.0f, 0.0f };
  ofFbo opticalFlowFbo;
  OpticalFlowShader opticalFlowShader;
  bool opticalFlowShaderLoaded { false };
  int startupFrame { -30 };

  ofFloatPixels opticalFlowPixels;
  bool cpuSamplingEnabled { false };

  ofParameterGroup parameters;
  ofParameter<float> minSpeedMagnitude {"MinSpeedMagnitude", 0.40f, 0.0f, 1.0f};

  bool videoVisible { false };
  bool motionVisible { false };
};
