#pragma once

#include <misc/cstring_utils.hh>
#include <rv/rv.hh>
#include <stddef.h>



class RvZone {
  int mLph = 0;
  char *mName = NULL;

public:

  int getLph() const {
    return mLph ? mLph : ZONE_LPH_DEFAULT;
  }
  bool setLph(int lph) {
    mLph = lph;
    return true;
  }

  const char* getName() const {
    return mName ? mName : "";
  }
  void setName(const char *name) {
    csu_assign(&mName, name);
  }

};
