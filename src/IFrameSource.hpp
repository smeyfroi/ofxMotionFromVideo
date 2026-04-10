#pragma once

#include "ofMain.h"

class IFrameSource {
public:
  virtual ~IFrameSource() = default;

  virtual void update() = 0;
  virtual void stop() = 0;
  virtual bool keyPressed(int key) { return false; }
  virtual void setPositionSeconds(int seconds) {}

  virtual bool isReady() const = 0;
  virtual bool isFrameNew() const = 0;
  virtual bool isMirrored() const { return false; }
  virtual glm::vec2 getSize() const = 0;

  virtual const ofFbo& getCurrentFrameFbo() const = 0;
  virtual const ofFbo& getPreviousFrameFbo() const = 0;
};
