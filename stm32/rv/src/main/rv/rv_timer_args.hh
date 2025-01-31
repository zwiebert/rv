/**
 * \file     rv/rv_timer_data.hh
 * \brief    basic data to program and run a zone timer
 */

#pragma once

#include <stdint.h>
#include <stddef.h>

typedef struct {
  char *first, *second;
} cstr_pair;

/// \brief   options for programming a zone timer
struct SetArgs {
  int8_t valve_number = -1; ///< valve number or -1 (vn)
  int8_t timer_number = 0; ///< timer number (tn)
  int16_t on_duration = 0; ///< on duration in seconds (d1)
  int16_t off_duration = 0; ///< off duration in seconds (d0)
  int16_t mTodSpanBegin = 0; ///<  time of day span begin in ??? (sb)
  int16_t mTodSpanEnd = 0; ///< time of day span end in ??? (se)
  int16_t period = 0; ///< repeat the on/off/repeats cycle ever PERIOD in ??? (per)
  int8_t repeats = 0; ///< repeat on/off-durations REPEATS times (r)
  int8_t mDaysInterval = 0; ///< run this timer every PERIOD days (di)
  bool ignoreRainSensor :1 = false;  ///< if true, force to run timer regardless of rain sensor state (ir)
  bool ignorePumpPause :1 = false;  ///< if true, force to run timer regardless of pump pause state (ip)

  /// \brief             convert args to JSON using abbreviated member names (see names in parants)
  /// \param buf         destination buffer provided by caller
  /// \param buf_size    size of buffer provided by caller
  /// \param no_brackets if true, don't surround our JSON with curly brackets
  char* toJSON(char *buf, size_t buf_size, bool no_brackets = false) const;
  /**
   * \brief             convert args to JSON using abbreviated member names (see names in parants)
   * \param sj          json builder
   * \return            success
   */
  bool to_json(class UoutBuilderJson &sj, bool content_only = false) const;
public:
  SetArgs() = default;
  SetArgs(cstr_pair kvs[], int len);
};

static_assert(sizeof (SetArgs) <= 16);
