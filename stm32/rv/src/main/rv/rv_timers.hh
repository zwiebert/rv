/**
 * \file  rv/rv_timers.hh
 * \brief A container and scheduler of zone timers
 */

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

/// \brief  Create, manage and schedule zone timers.
class RvTimers {
private:
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


public:
  RvTimers(switch_valve_cb cb) :
      mSvCb(cb) {
  }

public:

  /// \brief   Get count of existing zone timers (of any state)
  unsigned get_used_count() const {
    return mActiveTimers.size();
  }

  /// \brief   register callback to actuate any valve
  /// \param cb    a callback to switch multiple valves by bit-masks
  void register_callback(switch_valve_cb cb) {
    mSvCb = cb; // XXX
  }

  /// \brief      register callback to notify about any modified timer
  /// \param  cb  a callback to notify about change of timer by VALVE_NUMBER, TIMER_NUMBER and flag if timer was REMOVED
  void register_callback(timer_was_modified_cb cb) {
    mTwmCb = cb; // XXX
  }

  /// \brief     Create, overwrite or delete a timer by given options in ARGS
  ///
  /// If on_duration in ARGS is 0, then delete the existing timer with the same VN/TN
  ///
  /// \args      Describing the new timer by VALVER_NUMBER, TIMER_NUMBER and options.
  /// \return    A pointer to the created timer or NULL.
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
      if (mSvCb)
        timer->register_callback(mSvCb, args.valve_number);
      timer->set(args);
      if (mTwmCb)
        mTwmCb(args.valve_number, args.timer_number, false);
      return timer;
    }
    return 0;

  }

  /// \brief                Create, overwrite or delete a timer by given options
  ///
  /// If on_duration is 0, then delete the existing timer with the same VN/ID
  ///
  /// \valve_number,id      Describing the new timer by VALVER_NUMBER, TIMER_NUMBER and options.
  /// \return               A pointer to the created timer or NULL.
  RvTimer* set(int valve_number, int on_duration, int id = 0) {
    RvTimer::SetArgs args;
    args.on_duration = on_duration;
    args.valve_number = valve_number;
    args.timer_number = id;
    return set(args);
  }

  //// \brief   Unused function (XXX: Remove this function)
  void XXXunset(int valve_number, int id) {
    mActiveTimers.remove_if([&](RvTimer &vt) -> bool {
      return vt.match(valve_number, id);
    });

    if (mTwmCb)
      mTwmCb(valve_number, id, true);
  }

  /// \brief   Do work
  void loop();

public:
  /// \brief  Get all timers as pointer to a list
  const RvtList* getTimerList() const {
    return &mActiveTimers;
  }


private:
  RvtList mActiveTimers, mFreeTimers;
  switch_valve_cb mSvCb;
  timer_was_modified_cb mTwmCb = 0;
};

