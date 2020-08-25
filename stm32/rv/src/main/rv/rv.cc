#include "rv.hh"

#include "rv_zones.hh"

RvZones rvz;

int getLph(int valve_number) {
  int result = ZONE_LPH_DEFAULT;

  if (valve_number < RV_VALVE_COUNT)
    result = rvz[valve_number].getLph();

  return result;
}

bool setLph(int valve_number, int lph) {
  if (valve_number < RV_VALVE_COUNT) {
    return rvz[valve_number].setLph(lph);
  }
  return false;
}

