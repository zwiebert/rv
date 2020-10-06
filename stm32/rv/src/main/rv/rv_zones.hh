/**
 * \file   rv/rv_zones.hh
 * \brief  Array of all valves (= zones)
 */

#pragma once

#include <rv/rv.hh>
#include <rv/rv_zone.hh>
#include <array>

/// \brief Array of valves.
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
