#pragma once

#include <rv/rv.hh>
#include <rv/rv_zone.hh>
#include <array>


class RvZones
{
  std::array<RvZone, RV_VALVE_COUNT> mRvz;
public:
  RvZones() {
  }

  RvZone& operator[](int idx) {
    return mRvz[idx];
  }
};
