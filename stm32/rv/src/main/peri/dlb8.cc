#include "user_config.h"
#include "../../../Libraries/tm1638/include/boards/dlb8.h"

#include <libopencm3/stm32/rcc.h>
#include <libopencm3/stm32/gpio.h>
#include <libopencm3/stm32/usart.h>
#include <libopencm3/stm32/i2c.h>
#include <libopencm3/stm32/f1/memorymap.h>

#include <stdio.h>
#include <stdint.h>
#include <sys/_timeval.h>
#include <misc/int_macros.h>
#include <time.h>
#include "dlb8.h"

#define D(x) x

Dlb8 input[2];
Dlb8 *dlb8_obj[2];

// missing functions because of changed timer interface
void display_print_timers(); //XXX - missing
#define TIMER_SET_DONE 1 //XXX - missing
void valveTimer_incrementTimerDuration(int);
void valveTimer_finishTimer(int);
void timer_set(int);

void dlb8_print_date(Dlb8 *obj, struct tm *tm) {
    uint8_t digit = 0;
    uint8_t mask = 0x01;
    int n;

    n = tm->tm_year + 1900;

    digit = n / 1000;
    n %= 1000;
    dlb8_put_chars(obj, mask, '0' + digit, false);
    mask <<= 1;
    digit = n / 100;
    n %= 100;
    dlb8_put_chars(obj, mask, '0' + digit, false);
    mask <<= 1;
    digit = n / 10;
    n %= 10;
    dlb8_put_chars(obj, mask, '0' + digit, false);
    mask <<= 1;
    digit = n;
    dlb8_put_chars(obj, mask, '0' + digit, false);
    mask <<= 1;

    n = tm->tm_mon;
    digit = n > 9 ? n / 10 : 0;
    dlb8_put_chars(obj, mask, '0' + digit, false);
    mask <<= 1;
    digit = digit ? n % 10 : n;
    dlb8_put_chars(obj, mask, '0' + digit, false);
    mask <<= 1;

    n = tm->tm_mday;
    digit = n > 9 ? n / 10 : 0;
    dlb8_put_chars(obj, mask, '0' + digit, false);
    mask <<= 1;
    digit = digit ? n % 10 : n;
    dlb8_put_chars(obj, mask, '0' + digit, false);
    mask <<= 1;

}

void dlb8_print_time(Dlb8 *obj, struct tm *tm) {
    uint8_t digit = 0;
    uint8_t mask = 0x01;
    int n;

    n = tm->tm_hour;
    digit = n > 9 ? n / 10 : 0;
    dlb8_put_chars(obj, mask, '0' + digit, false);
    mask <<= 1;
    digit = digit ? n % 10 : n;
    dlb8_put_chars(obj, mask, '0' + digit, false);
    mask <<= 1;

    n = tm->tm_min;
    digit = n > 9 ? n / 10 : 0;
    dlb8_put_chars(obj, mask, '0' + digit, false);
    mask <<= 1;
    digit = digit ? n % 10 : n;
    dlb8_put_chars(obj, mask, '0' + digit, false);
    mask <<= 1;

    for (int i = 0; i < 4; ++i) {
        dlb8_put_chars(obj, mask, ' ', false);
        mask <<= 1;
    }
}

void dlb8_loop() {
  uint8_t button = dlb8_get_changed_buttons(dlb8_obj[0]);

  if (button) {
    D(printf("pressed button: %x\n", button));
    for (int i = 0; i < 8; ++i) {
      if (GET_BIT(button, i))
        valveTimer_incrementTimerDuration(i);
    }
    timer_set(TIMER_SET_DONE);
  }
  button = dlb8_calculate_hold_buttons(dlb8_obj[0], 200);

  if (button) {
    D(printf("hold button: %x\n", button));
    for (int i = 0; i < 8; ++i) {
      if (GET_BIT(button, i))
        valveTimer_finishTimer(i);
    }
  }

  if ((button & 0x81) == 0x81) {
    time_t t = time(0);
    struct tm *tm = gmtime(&t);
    D(printf("%d-%d-%dT%d:%d:%02d\n", tm->tm_year + 1900, tm->tm_mon, tm->tm_mday, tm->tm_hour, tm->tm_min, tm->tm_sec));
    dlb8_print_date(dlb8_obj[0], tm);
  }

  if ((button & 0x41) == 0x41) {
    time_t t = time(0);
    struct tm *tm = gmtime(&t);
    D(printf("%d-%d-%dT%d:%d:%02d\n", tm->tm_year + 1900, tm->tm_mon, tm->tm_mday, tm->tm_hour, tm->tm_min, tm->tm_sec));
    dlb8_print_time(dlb8_obj[0], tm);
  }
  if ((button & 0x21) == 0x21) {
    display_print_timers();

  }
}

void dlb8_setup(void) {
    rcc_periph_clock_enable(RCC_GPIOB);
    Tm1638_setup(GPIOB, GPIO13, GPIOB, GPIO14);
    Tm1638_construct(&input[0].tm, GPIOB, GPIO15);
    dlb8_obj[0] = &input[0];
    Tm1638_construct(&input[1].tm, GPIOB, GPIO12);
    dlb8_obj[1] = &input[1];
}
