/*
 * uart.c
 *
 *  Created on: 15.05.2019
 *      Author: bertw
 */


#include <libopencm3/stm32/rcc.h>
#include <libopencm3/stm32/gpio.h>
#include <libopencm3/stm32/usart.h>

#include <libopencm3/stm32/rcc.h>
#include <libopencm3/stm32/gpio.h>
#include <libopencm3/stm32/usart.h>
#include <libopencm3/stm32/i2c.h>
#include <libopencm3/stm32/f1/memorymap.h>

#include <libopencm3/stm32/rcc.h>
#include <libopencm3/stm32/gpio.h>
#include <libopencm3/stm32/usart.h>
#include <libopencm3/cm3/nvic.h>
#include <libopencm3/cm3/scb.h>

#include <string.h>
#include "../loop.hh"

#define RX_BUFSIZE 128
static volatile uint8_t buf[RX_BUFSIZE];
static volatile uint8_t head = 0, tail = 0;

static void rx_put(uint8_t c) {
	buf[tail++] = c;
	tail &= (RX_BUFSIZE - 1);
}

int esp32_getc(void) {
	int result = -1;

	if (head != tail) {
		result = buf[head++] & 0xFF;
		head  &= (RX_BUFSIZE - 1);
	}

	return result;
}


void USART1_IRQHandler(void);

void USART1_IRQHandler(void) {
	uint16_t data;
	/* Check if we were called because of RXNE. */
	if (((USART_CR1(USART1) & USART_CR1_RXNEIE) != 0) &&
	    ((USART_SR(USART1) & USART_SR_RXNE) != 0)) {

		/* Retrieve the data from the peripheral. */
		data = usart_recv(USART1);

		rx_put(data & 0xff);
		lf_setBit(lf_cli);

	}
}


void uart_setup(void) {
	/* Enable the USART2 interrupt. */
	nvic_enable_irq(NVIC_USART1_IRQ);
 	rcc_periph_clock_enable(RCC_GPIOA);
 	rcc_periph_clock_enable(RCC_USART1);
 	gpio_set_mode(GPIO_BANK_USART1_TX, GPIO_MODE_OUTPUT_50_MHZ, GPIO_CNF_OUTPUT_ALTFN_PUSHPULL, GPIO_USART1_TX);
	gpio_set_mode(GPIO_BANK_USART1_RX, GPIO_MODE_INPUT, GPIO_CNF_INPUT_FLOAT, GPIO_USART1_RX);

 	/* Setup UART parameters. */
 	usart_set_baudrate(USART1, 115200);
 	usart_set_databits(USART1, 8);
 	usart_set_stopbits(USART1, USART_STOPBITS_1);
 	usart_set_mode(USART1, USART_MODE_TX_RX);
 	usart_set_parity(USART1, USART_PARITY_NONE);
 	usart_set_flow_control(USART1, USART_FLOWCONTROL_NONE);

	/* Enable USART2 Receive interrupt. */
	USART_CR1(USART1) |= USART_CR1_RXNEIE;

 	/* Finally enable the USART. */
 	usart_enable(USART1);
}

int esp32_write(const char *data, unsigned data_len) {
	int i;

	{
		for (i = 0; i < data_len; i++)
			usart_send_blocking(USART1, data[i]);
		return i;
	}
	return -1;
}

int esp32_puts(const char *s) {
	return esp32_write(s, strlen(s));
}

int esp32_read(char *buf, unsigned buf_size) {
	int i;

	for (i = 0; i < buf_size; ++i) {
		int c = esp32_getc();
		if (c == -1)
			break;
		buf[i] = c;

	}
	return i;
}



