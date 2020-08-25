#pragma once

#include <misc/int_macros.h>
#include <rv/rv_timer.hh>
#include <rv/rv_timer_data.hh>

#include <stdint.h>

typedef void (*switch_valves_cb)(uint16_t valve_bits, uint16_t valve_mask);

#define IGNORE_RAIN_SENSOR 1
#define IGNORE_PUMP_PAUSE 2

#include "allocator_malloc.hh"
#include <list>
template<class T> using TList = std::list<T, AllocatorMalloc<T>>;
typedef TList<RvTimer> RvtList;

//////////////////////////////////////////////////////////////////////////////////////////////////

class RvTimers {
  static uint16_t valve_bits, valve_mask;
  static void switch_valve_2(int valve_number, bool state) {
    SET_BIT(valve_mask, valve_number);
    if (state)
      SET_BIT(valve_bits, valve_number);
  }
private:
  RvtList mActiveTimers;

  RvTimer* get_timer() {
    mActiveTimers.emplace_back();
    return &mActiveTimers.back();
  }

  switch_valve_cb mSvCb; // XXX
  switch_valves_cb mSvsCb;

public:

  unsigned get_used_count() {
    return mActiveTimers.size();
  }
  //unsigned get_free_count() { return mRvTimers.get_free_count(); }

  RvTimers(switch_valve_cb cb, switch_valves_cb cb2) :
      mSvCb(cb), mSvsCb(cb2) {

  }

  void register_callback(switch_valve_cb cb) {
    mSvCb = cb; // XXX
  }

  RvTimer* set(RvTimer::SetArgs &args, int valve_number, int id) {
    for (RvTimer &vt : mActiveTimers) {
      if (vt.match(valve_number, id)) {
        vt.changeState(RvTimer::STATE_DONE);
        break;
      }
    }

    RvTimer *timer = get_timer();
    if (!timer)
      return 0;

    if (args.on_duration == 0) {
      timer->changeState(RvTimer::STATE_DONE);
    }

    timer->register_callback((mSvCb ? mSvCb : switch_valve_2), valve_number);
    timer->set(args, id);
    return timer;

  }

  RvTimer *set(int valve_number, int on_duration, int id = 0) {
    RvTimer::SetArgs args;
    args.on_duration = on_duration;
    return set(args, valve_number, id);
  }

  void unset(int valve_number, int id) {
    mActiveTimers.remove_if([&](RvTimer &vt) -> bool {  return vt.match(valve_number, id); });
  }

  void loop();

public:
  RvtList *getTimerList() {
    return &mActiveTimers;
  }
};

