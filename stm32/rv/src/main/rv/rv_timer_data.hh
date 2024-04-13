/**
 * \file     rv/rv_timer_data.hh
 * \brief    basic data to program and run a zone timer
 */

#pragma once


#include "rv_timer_args.hh"

#include <stdint.h>
#include <stddef.h>

/// \brief  parameter to program and run a zone timer
struct RvTimerData {
protected:
  SetArgs mArgs;
  time_t mNextRun = 0;
  time_t mLastRun = 0;

public:

  char* toJSON(char *buf, size_t buf_size, bool no_brackets = false) const;
  bool to_json(class UoutBuilderJson &sj) const;
  RvTimerData() {
  }
};

