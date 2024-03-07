/**
 * \file   loop/loop.hh
 * \brief  App main loop scheduler (flag-bit based).
 */

#pragma once

#define loop_flags_periodic 0 // disable periodic intervals


enum loop_flagbits {
  lf_interval_1s,
  lf_interval_512ms,
  lf_interval_64ms,
  lf_cli,
  lf_wpl,
  lf_fetchData,
  lf_rx_buffer_full,
  /////////////
  lf_Len
};


extern volatile int loop_flags_once;
#ifndef loop_flags_periodic
extern volatile int loop_flags_periodic;

inline void lfPer_setBit(enum loop_flagbits v) {
  loop_flags_periodic = (loop_flags_once | (1 << v));
}

inline void lfPer_setBits(unsigned bit_mask) {
  loop_flags_periodic = (loop_flags_once | bit_mask);
}
#endif

inline void lf_setBit(enum loop_flagbits v) {
  loop_flags_once = (loop_flags_once | (1 << v));
}

inline void lf_setBits(unsigned bit_mask) {
  loop_flags_once = (loop_flags_once | bit_mask);
}

#ifdef __cplusplus
extern "C" {
#endif

void lf_loop();

extern bool got_zoneData;

#ifdef __cplusplus
}
#endif
