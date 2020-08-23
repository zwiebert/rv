#pragma once

#define loop_flags_periodic 0 // not used

#define lf_setBits(v) (loop_flags_once |= (v))
#define lf_setBit(v)  lf_setBits(1<<(v))
#define lfPer_setBits(v) (loop_flags_periodic |= (v))
#define lfPer_setBit(v)  lfPer_setBits(1<<(v))

enum loop_flagbits {
  lf_interval_1s,
  lf_interval_512ms,
  lf_interval_64ms,
  lf_cli,
  lf_wpl,
  lf_fetchData,
  /////////////
  lf_Len
};

extern volatile int loop_flags_once;
#ifndef loop_flags_periodic
extern volatile int loop_flags_periodic;
#endif

#ifdef __cplusplus
extern "C" {
#endif

void lf_loop();

extern bool got_zoneData;

#ifdef __cplusplus
}
#endif
