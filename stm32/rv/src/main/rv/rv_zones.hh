#pragma once

#include "rv_zone.hh"
#include "list.hh"
#include <vector>

template <typename T> using TVector = std::vector<T,my_allocator<T>>;

class RvZones //: public TVector<RvZone>
{
  RvZone mRvz[RV_VALVE_COUNT];
  //TVector<RvZone> mRvz = TVector<RvZone>(RV_VALVE_COUNT);
public:
  RvZones() //: TVector<RvZone>(RV_VALVE_COUNT)
{  }

  RvZone& operator[](int idx) { return mRvz[idx]; }
};
