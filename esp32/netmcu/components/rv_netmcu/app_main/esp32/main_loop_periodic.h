/**
 * \file   app_main/esp32/main_loop_periodic.h
 * \brief  Run events in main task by setting event bits from periodic 100ms timer
 */
#pragma once

#include "app_config/proj_app_cfg.h"

#include <utils_misc/int_types.h>
#include <utils_misc/int_macros.h>

union lfPerFlags {
  struct {
    bool lf_loopWatchDog :1;
    bool lf_loopCli :1;
  } flags;
  unsigned bitmask;
};

extern lfPerFlags mainLoop_PeriodicFlags;

void lfPer100ms_mainFun();
