/*
 * rtc.c
 *
 *  Created on: 01.05.2019
 *      Author: bertw
 */
#include <libopencm3/stm32/rcc.h>
#include <libopencm3/stm32/gpio.h>
#include <libopencm3/stm32/rtc.h>
#include <libopencm3/stm32/usart.h>
#include <libopencm3/stm32/pwr.h>
#include <libopencm3/cm3/nvic.h>

#define OUTPUT_PIN_COUNT 16
#define MAX_TIMER_MINUTES 60
#define NO_TIMER_VALUE ~0UL

uint32_t timers[OUTPUT_PIN_COUNT];
void (*timer_set_cb)(int8_t channel);
void (*timer_alarm_cb)(int8_t channel);

volatile uint32_t next_timer_value;
volatile uint32_t curr_time;
volatile bool timer_rang, timer_noticed;

uint32_t rtc_set_timer_duration_by_minutes(uint8_t channel, uint8_t minutes) {
	if (!(channel < OUTPUT_PIN_COUNT && minutes <= MAX_TIMER_MINUTES && curr_time > 0))
		return 0;
	if (timer_set_cb)
		timer_set_cb(channel);

	uint32_t timer = timers[channel] = minutes * 60 + curr_time;

	if (timer < next_timer_value) {
		next_timer_value = timer;
	}
	printf("timer set: ch=%u, end=%u, curr=%u, next:%u\n",
			(unsigned)channel, (unsigned)timer, (unsigned)curr_time, (unsigned)next_timer_value);
	return timer;
}

bool rtc_timer_test() {
	return next_timer_value < curr_time;
}

void rtc_update_timer_table(void) {
	next_timer_value = NO_TIMER_VALUE;
	bool callback_called = false;

	for (uint8_t i = 0; i < OUTPUT_PIN_COUNT; ++i) {
		uint32_t timer = timers[i];
		if (timer < curr_time) {
			timers[i] = NO_TIMER_VALUE;
			printf("timer alarm: ch=%u, end=%u, curr=%u\n", (unsigned) i,
					(unsigned) timer, (unsigned) curr_time);
			timer_alarm_cb(i);
			callback_called = true;

		} else if (timer < next_timer_value) {
			next_timer_value = timer;
		}
	}
	if (callback_called) {
		timer_alarm_cb(-1);
	}
}

void rtc_timer_loop(void) {
	if (timer_rang && !timer_noticed) {
		rtc_update_timer_table();
		timer_noticed = true;
	}
}

static void timer_table_setup(void) {
	next_timer_value = NO_TIMER_VALUE;
	for (int i=0; i < OUTPUT_PIN_COUNT; ++i) {
	   timers[i] = NO_TIMER_VALUE;
	}
}

static void nvic_setup(void)
{
	/* Without this the RTC interrupt routine will never be called. */
	nvic_enable_irq(NVIC_RTC_IRQ);
	nvic_set_priority(NVIC_RTC_IRQ, 1);
}

void RTC_IRQHandler(void)
{
	volatile uint32_t j = 0, c = 0;

	/* The interrupt flag isn't cleared by hardware, we have to do it. */
	rtc_clear_flag(RTC_SEC);

	/* Visual output. */
	gpio_toggle(GPIOC, GPIO13);

	curr_time = rtc_get_counter_val();

	if (timer_rang && timer_noticed) {
		timer_rang = false;
		timer_noticed = false;
	}

	if (next_timer_value < curr_time) {
		timer_rang = true;
	}
return;
	c = rtc_get_counter_val();


	/* Display the current counter value in binary via USART1. */
	for (j = 0; j < 32; j++) {
		if ((c & (0x80000000 >> j)) != 0) {
			usart_send_blocking(USART1, '1');
		} else {
			usart_send_blocking(USART1, '0');
		}
	}
	usart_send_blocking(USART1, '\n');
	usart_send_blocking(USART1, '\r');
}


void rtc_setup(void)
{
	timer_table_setup();
	curr_time = rtc_get_counter_val();
	/*
	 * If the RTC is pre-configured just allow access, don't reconfigure.
	 * Otherwise enable it with the LSE as clock source and 0x7fff as
	 * prescale value.
	 */
	rtc_auto_awake(RCC_LSE, 0x7fff);

	/* Setup the RTC interrupt. */
	nvic_setup();

	/* Enable the RTC interrupt to occur off the SEC flag. */
	rtc_interrupt_enable(RTC_SEC);
}
