#pragma once

#define lf_setBits(v) (loop_flags_once |= (v))
#define lf_setBit(v)  lf_setBits(1<<(v))
#define lfPer_setBits(v) (loop_flags_periodic |= (v))
#define lfPer_setBit(v)  lfPer_setBits(1<<(v))

enum loop_flagbits {
  lf_wpl, lf_cli, lf_cxx,
  lf_ic2c_check,
  /////////////
  lf_Len
};

extern int loop_flags_once;
extern int loop_flags_periodic;

#ifdef __cplusplus
extern "C" {
#endif

void lf_loop();

#ifdef __cplusplus
}
#endif
