#pragma once

#include <misc/int_macros.h>
#include <rv/rv_timer.hh>
#include <rv/rv_timer_data.hh>
#include <algorithm>
#include <stdint.h>

typedef void (*switch_valves_cb)(uint16_t valve_bits, uint16_t valve_mask);
typedef void (*timer_was_modified_cb)(int vn, int tn, bool removed);

#include "rv_allocator.hh"
#include <list>
template<class T> using TList = std::list<T, RvAllocator<T>>;
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
  RvtList mActiveTimers, mFreeTimers;

  RvTimer* create_timer() {
    if (mFreeTimers.empty()) {
      mActiveTimers.emplace_back();
    } else {
      auto src = mFreeTimers.begin();
      mActiveTimers.splice(mActiveTimers.end(), mFreeTimers, src);
    }
    return &mActiveTimers.back();
  }

  void destroy_timer(RvtList::iterator src) {
    const int vn = src->getValveNumber();
    const int tn = src->getTimerNumber();
    *src = {};
    mFreeTimers.splice(mFreeTimers.end(), mActiveTimers, src);
    if (mTwmCb)
      mTwmCb(vn, tn, true);
  }

  void destroy_timer(RvTimer *timer) {
    destroy_timer(std::find(mActiveTimers.begin(), mActiveTimers.end(), *timer));
  }

  RvTimer* find_timer(int vn, int tn) {
    for (RvTimer &vt : mActiveTimers) {
      if (vt.match(vn, tn))
        return &vt;
    }
    return nullptr;
  }


  switch_valve_cb mSvCb; // XXX
  switch_valves_cb mSvsCb;
  timer_was_modified_cb mTwmCb = 0;

public:

  unsigned get_used_count() const {
    return mActiveTimers.size();
  }
  //unsigned get_free_count() { return mRvTimers.get_free_count(); }

  RvTimers(switch_valve_cb cb, switch_valves_cb cb2) :
      mSvCb(cb), mSvsCb(cb2) {

  }

  void register_callback(switch_valve_cb cb) {
    mSvCb = cb; // XXX
  }
  void register_callback(timer_was_modified_cb cb) {
    mTwmCb = cb; // XXX
  }

  RvTimer* set(RvTimer::SetArgs &args) {

    for (auto it = mActiveTimers.begin(); it != mActiveTimers.end(); ++it) {
      if (it->match(args.valve_number, args.timer_number)) {
        it->changeState(RvTimer::STATE_DONE);
        rvt.destroy_timer(it);
        break;
      }
    }

    if (args.on_duration == 0)
      return 0;
    if (rs.getState() && !(args.period || args.mDaysInterval || args.ignoreRainSensor))
      return 0;

    if (RvTimer *timer = create_timer()) {
      timer->register_callback((mSvCb ? mSvCb : switch_valve_2), args.valve_number);
      timer->set(args);
      if (mTwmCb)
        mTwmCb(args.valve_number, args.timer_number, false);
      return timer;
    }
    return 0;

  }

  RvTimer* set(int valve_number, int on_duration, int id = 0) {
    RvTimer::SetArgs args;
    args.on_duration = on_duration;
    args.valve_number = valve_number;
    args.timer_number = id;
    return set(args);
  }

  void unset(int valve_number, int id) {
    mActiveTimers.remove_if([&](RvTimer &vt) -> bool {
      return vt.match(valve_number, id);
    });

    if (mTwmCb)
      mTwmCb(valve_number, id, true);
  }

  void loop();

public:
  const RvtList* getTimerList() const {
    return &mActiveTimers;
  }
};

