#include <rv/rain_sensor.hh>
#include <rv/rv.hh>
#include <rv/rv_timers.hh>
#include <rv/rv_zone.hh>
#include <rv/rv_zones.hh>
#include <rv/valve_relays.h>


RvTimers rvt = RvTimers(0, app_switch_valves);
RainSensor rs;
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

