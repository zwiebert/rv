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
#include <valve_timer.h>

#define OUTPUT_PIN_COUNT 16
#define MAX_TIMER_MINUTES 60
#define NO_TIMER_VALUE ~0UL

uint32_t timers[OUTPUT_PIN_COUNT];
void (*valve_timer_set_cb)(int8_t channel);
void (*valve_timer_alarm_cb)(int8_t channel);

volatile uint32_t next_timer_value;
volatile uint32_t curr_time;
volatile bool timer_rang, timer_noticed;


static void nvic_setup(void)
{
	/* Without this the RTC interrupt routine will never be called. */
	nvic_enable_irq(NVIC_RTC_IRQ);
	nvic_set_priority(NVIC_RTC_IRQ, 1);
}

void RTC_IRQHandler(void)
{
	/* The interrupt flag isn't cleared by hardware, we have to do it. */
	rtc_clear_flag(RTC_SEC);

	/* Visual output. */
	gpio_toggle(GPIOC, GPIO13);
	curr_time = rtc_get_counter_val();

	valve_timer_tick();
}


void rtc_setup(void)
{

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
