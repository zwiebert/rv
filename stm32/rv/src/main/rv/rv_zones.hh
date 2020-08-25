#pragma once

#include <rv/rv.hh>
#include <rv/rv_zone.hh>

class RvZones
{
  RvZone mRvz[RV_VALVE_COUNT];
public:
  RvZones() {
  }

  RvZone& operator[](int idx) {
    return mRvz[idx];
  }
};
