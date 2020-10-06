#ifdef MCU_HOST
#include "../real_time_clock.h"
#include <time.h>

static time_t start_time;




static struct Init {
  Init() { start_time = time(0); }
} init;


run_time_T runTime() {
  return time(0) - start_time;
}

time_t currentTime() {
  return time(0);
}







#endif

