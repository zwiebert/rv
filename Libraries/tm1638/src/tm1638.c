/*
 * tm1638.c
 *
 *  Created on: 02.05.2019
 *      Author: bertw
 */

#include "../include/tm1638.h"
#include <string.h>
#include <ctype.h>
#include <libopencm3/stm32/gpio.h>

uint32_t Tm1638_clk_port, Tm1638_dio_port;
uint16_t Tm1638_clk_pin, Tm1638_dio_pin;

#define TM1638_DATA_1 0
#define TM1638_DATA_0 1

static void pin_change_delay() {
	for (unsigned long i = 0; i < 300; ++i) {
		__asm__("nop");
	}
}

static void Tm1638_dout(Tm1638 *obj, bool value) {
	pin_change_delay();
	if (value)
		gpio_clear(Tm1638_dio_port, Tm1638_dio_pin);
	else
		gpio_set(Tm1638_dio_port, Tm1638_dio_pin);
	pin_change_delay();
}

static void Tm1638_strobe(Tm1638 *obj, bool value) {
	pin_change_delay();
	if (value)
		gpio_clear(obj->mStrobePort, obj->mStrobePin);
	else
		gpio_set(obj->mStrobePort, obj->mStrobePin);
	pin_change_delay();
}

static void Tm1638_clock(Tm1638 *obj, bool value) {
	pin_change_delay();
	if (value)
		gpio_set(Tm1638_clk_port, Tm1638_clk_pin);
	else
		gpio_clear(Tm1638_clk_port, Tm1638_clk_pin);
	pin_change_delay();
}

static void Tm1638_write_byte(Tm1638 *obj, uint8_t data) {
	for (int i = 0; i < 8; (++i), (data >>= 1)) {
		Tm1638_clock(obj, 0);
		Tm1638_dout(obj, (data & 0x01) ? TM1638_DATA_1 : TM1638_DATA_0);
		Tm1638_clock(obj, 1);
	}
}

bool Tm1638_write(Tm1638 *obj, const uint8_t *cmd, uint8_t cmd_len,
		const uint8_t *data, uint8_t data_len) {

	for (uint8_t i = 0; i < cmd_len; ++i) {
		Tm1638_strobe(obj, true);
		Tm1638_write_byte(obj, cmd[i]);
		if (i + 1 < cmd_len)
			Tm1638_strobe(obj, false);
	}

	if (!cmd_len)
		Tm1638_strobe(obj, true);

	for (uint8_t i = 0; i < data_len; ++i) {
		Tm1638_write_byte(obj, data[i]); // FIXME: inverted data
	}

	Tm1638_strobe(obj, false);

	return true;
}

bool Tm1638_read(Tm1638 *obj, uint8_t *buf, uint8_t len);

uint32_t Tm1638_read_keys(Tm1638 *obj);

#define S7A 0x01
#define S7B 0x02
#define S7C 0x04
#define S7D 0x08
#define S7E 0x10
#define S7F 0x20
#define S7G 0x40
#define S7P 0x80

static uint8_t Tm1638_char_to_7s(char c) {
	switch (c) {
	case '0':
		return S7A | S7B | S7C | S7D | S7E | S7F;
	case '1':
		return S7B | S7C;
	case '2':
		return S7A | S7B | S7G | S7D | S7E;
	case '3':
		return S7A | S7B | S7C | S7D | S7G;
	case '4':
		return S7B | S7C | S7G | S7F;
	case '5':
		return S7A | S7C | S7D | S7F | S7G;
	case '6':
		return S7A | S7C | S7D | S7E | S7F | S7G;
	case '7':
		return S7A | S7B | S7C;
	case '8':
		return S7A | S7B | S7C | S7D | S7E | S7F | S7G;
	case '9':
		return S7A | S7B | S7C | S7D | S7F | S7G;
	case 'a':
	case 'A':
		return S7A | S7B | S7C | S7E | S7F | S7G;
	case 'b':
	case 'B':
		return S7C | S7D | S7E | S7F | S7G;
	case 'c':
	case 'C':
		return S7A | S7D | S7E | S7F;
	case 'd':
	case 'D':
		return S7B | S7C | S7D | S7E | S7G;
	case 'e':
	case 'E':
		return S7A | S7D | S7E | S7F | S7G;
	case 'f':
	case 'F':
		return S7A | S7E | S7F | S7G;

	default:
		return 0;

	}
}

bool Tm1638_put_char(Tm1638 *obj, char c, uint8_t register_number) {


	uint8_t cmd[] = {
			TM1638_CMD_TYPE_DATA_MANAGEMENT | TM1638_CMD_DIR_WRITE
			| TM1638_CMD_ADDR_MODE_SINGLE,
	TM1638_CMD_TYPE_REGISTER_ADDRESS | register_number,

	};

	uint8_t data[] = {Tm1638_char_to_7s(c)};

	Tm1638_write(obj, cmd, sizeof cmd, data, sizeof data);

	return true;
}

void Tm1638_clear_registers(Tm1638 *obj) {
	uint8_t cmd[] = {
			TM1638_CMD_TYPE_DISPLAY_CONTROL | TM1638_DISPLAY_ON | TM1638_DISPLAY_DIMMER(2),
			TM1638_CMD_TYPE_DATA_MANAGEMENT | TM1638_CMD_DIR_WRITE
			| TM1638_CMD_ADDR_MODE_INCR,

	TM1638_CMD_TYPE_REGISTER_ADDRESS | 0x0,

	};

	uint8_t data[16] = {0};

	Tm1638_write(obj, cmd, sizeof cmd, data, sizeof data);
}

Tm1638 *Tm1638_construct(void *memory, uint32_t stb_port, uint16_t stb_pin) {
	Tm1638 *obj = memory;
	memset(obj, 0, sizeof(*obj));
	gpio_set(stb_port,stb_pin);
	gpio_set_mode(stb_port, GPIO_MODE_OUTPUT_2_MHZ, GPIO_CNF_OUTPUT_OPENDRAIN, stb_pin);

	obj->mStrobePort = stb_port;
	obj->mStrobePin = stb_pin;
	Tm1638_clear_registers(obj);
	return obj;
}
