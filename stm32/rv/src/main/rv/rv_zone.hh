#pragma once

#include "rv.hh"
#include <string>

class RvZone {
  int mLph = 0;
  std::string mName;
public:
  int getLph() {
    return mLph ? mLph : ZONE_LPH_DEFAULT;
  }
  bool setLph(int lph) {
    mLph = lph;
    return true;
  }
  std::string &name() { return mName; }
};
