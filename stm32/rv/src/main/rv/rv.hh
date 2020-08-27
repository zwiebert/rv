#pragma once


class RvTimerPause;
class RvTimerData;
class RvTimer;
class RvTimers;
class RainSensor;
class RvZone;
class RvZones;

extern RvTimers rvt;
extern RainSensor rs;
extern RvZones rvz;

//int getLph(int valve_number);
//bool setLph(int valve_number, int lph);

#define PAUSE_SECS_PER_LITER 8
#define PAUSE_AFTER_LITER 100
#define PAUSE_LPH_THRESHOLD 100
#define ZONE_LPH_DEFAULT 1000

#define RV_TIMER_COUNT 20
#define RV_VALVE_COUNT  14
#define RV_MAX_LPH 1600
