/*
 * tm1638.c
 *
 *  Created on: 02.05.2019
 *      Author: bertw
 */

#include "../include/tm1638.h"
#include <string.h>
#include <ctype.h>

#ifdef STM32F1
#include <libopencm3/stm32/gpio.h>
#endif

#include "utils_misc/int_macros.h"

gpio_pin_T Tm1638_clk_pin, Tm1638_dio_pin;

#define TM1638_DATA_1 0
#define TM1638_DATA_0 1

#define D(x)

static void pin_change_delay() {
	for(int i = 0; i < 3; ++i) {
		__asm__("nop");
	}
}

static void twait() {
	for(int i = 0; i < 5; ++i) {
		__asm__("nop");
	}
}


static void Tm1638_dout(Tm1638 *, bool value) {
	my_put_pin(Tm1638_dio_pin, !value);
}

static bool Tm1638_din(Tm1638 *obj) {
	uint16_t result = my_get_pin(Tm1638_dio_pin);
	return result != 0;
}

static void Tm1638_strobe(Tm1638 *obj, bool value) {
	pin_change_delay();
	my_put_pin(obj->mStrobePin, !value);
	pin_change_delay();
}

static void Tm1638_clock(Tm1638 *obj, bool value) {
	pin_change_delay();
	my_put_pin(Tm1638_clk_pin, value);
	pin_change_delay();
}

static void Tm1638_write_byte(Tm1638 *obj, uint8_t data) {
	D(printf("write byte: %x\n", data));
	for (int i = 0; i < 8; (++i), (data >>= 1)) {
		Tm1638_clock(obj, 0);
		Tm1638_dout(obj, (data & 0x01) ? TM1638_DATA_1 : TM1638_DATA_0);
		Tm1638_clock(obj, 1);
	}
}

static uint8_t Tm1638_read_byte(Tm1638 *obj) {
	uint8_t data = 0;
	for (int i = 0; i < 8; ++i) {
		Tm1638_clock(obj, 0);
		if (Tm1638_din(obj))
			SET_BIT(data, i);
		Tm1638_clock(obj, 1);
	}
	return data;
}

bool Tm1638_write(Tm1638 *obj, const uint8_t *cmd, uint8_t cmd_len,
		const uint8_t *data, uint8_t data_len, const uint8_t *regs) {

	for (uint8_t i = 0; i < cmd_len; ++i) {
		Tm1638_strobe(obj, true);
		Tm1638_write_byte(obj, cmd[i]);
		if (regs || i + 1 < cmd_len || !data_len) {
			Tm1638_strobe(obj, false);
		}
	}

	if (data_len) {

		if (regs) {
			for (uint8_t i = 0; i < data_len; ++i) {
				Tm1638_strobe(obj, true);
				Tm1638_write_byte(obj,
						TM1638_CMD_TYPE_REGISTER_ADDRESS | regs[i]);
				Tm1638_write_byte(obj, data[i]);
				Tm1638_strobe(obj, false);
			}
			return true;
		} else {
			if (!cmd_len) {
				Tm1638_strobe(obj, true);
			}

			for (uint8_t i = 0; i < data_len; ++i) {
				Tm1638_write_byte(obj, data[i]);
			}

			Tm1638_strobe(obj, false);
		}
	}

	return true;
}



#define S7A 0x01
#define S7B 0x02
#define S7C 0x04
#define S7D 0x08
#define S7E 0x10
#define S7F 0x20
#define S7G 0x40
#define S7P 0x80

uint8_t Tm1638_char_to_7s(char c) {
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
	case '-':
	    return S7G;
	case '_':
		return S7D;

	default:
		return 0;

	}
}

bool Tm1638_put_data(Tm1638 *obj, const uint8_t data[],  uint8_t length, const uint8_t registers[]){
	uint8_t cmd[] = {
			TM1638_CMD_TYPE_DATA_MANAGEMENT | TM1638_CMD_DIR_WRITE
			| TM1638_CMD_ADDR_MODE_SINGLE
	};
	if (!length || !registers || registers[0] == 0xff)
		return false;

	return Tm1638_write(obj, cmd, sizeof cmd, data, length, registers);
}


bool Tm1638_put_value(Tm1638 *obj, uint8_t value, uint8_t register_number) {


	uint8_t cmd[] = {
			TM1638_CMD_TYPE_DATA_MANAGEMENT | TM1638_CMD_DIR_WRITE
			| TM1638_CMD_ADDR_MODE_SINGLE,
	TM1638_CMD_TYPE_REGISTER_ADDRESS | register_number,

	};

	uint8_t data[] = {value};

	Tm1638_write(obj, cmd, sizeof cmd, data, sizeof data, 0);

	return true;
}

bool Tm1638_put_char(Tm1638 *obj, char c, uint8_t register_number) {
	return Tm1638_put_value(obj, Tm1638_char_to_7s(c), register_number);
}

void Tm1638_clear_registers(Tm1638 *obj) {
	uint8_t cmd[] = {
			TM1638_CMD_TYPE_DISPLAY_CONTROL | TM1638_DISPLAY_ON | TM1638_DISPLAY_DIMMER(2),
			TM1638_CMD_TYPE_DATA_MANAGEMENT | TM1638_CMD_DIR_WRITE
			| TM1638_CMD_ADDR_MODE_INCR,

	TM1638_CMD_TYPE_REGISTER_ADDRESS | 0x0,

	};

	uint8_t data[16] = {0};

	Tm1638_write(obj, cmd, sizeof cmd, data, sizeof data, 0);
}


bool Tm1638_detect(Tm1638 *obj) {
  bool dio_pullup_recognized;
  my_pin_input_pull_down(Tm1638_dio_pin);
  dio_pullup_recognized =  my_get_pin(Tm1638_dio_pin);
  my_pin_output_open(Tm1638_dio_pin);
  return dio_pullup_recognized;
}

uint32_t Tm1638_read(Tm1638 *obj) {
	uint32_t result = 0;

	Tm1638_strobe(obj, true);
	Tm1638_write_byte(obj,
			TM1638_CMD_TYPE_DATA_MANAGEMENT | TM1638_CMD_DIR_READ);
	my_pin_input_pull_down(Tm1638_dio_pin);
	twait();
	for (int i = 0; i < 4; ++i) {
		uint32_t data = Tm1638_read_byte(obj);
		if (data)
			result |= data << (i*8);

	}
	Tm1638_strobe(obj, false);
	my_pin_output_open(Tm1638_dio_pin);

	return result;

}

#ifdef STM32F1

Tm1638 *Tm1638_construct(void *memory, uint32_t stb_port, uint16_t stb_pin) {
	Tm1638 *obj = memory;
	memset(obj, 0, sizeof(*obj));

	obj->mStrobePin.port = stb_port;
	obj->mStrobePin.pin = stb_pin;

	my_set_pin(obj->mStrobePin);
	my_pin_output_open(obj->mStrobePin);

	Tm1638_clear_registers(obj);
	return obj;
}


void Tm1638_setup(uint32_t clk_port, uint16_t clk_pin, uint32_t dio_port, uint16_t dio_pin) {
	 Tm1638_clk_pin.port = clk_port;
	 Tm1638_dio_pin.port = dio_port;

	 Tm1638_clk_pin.pin = clk_pin;
	 Tm1638_dio_pin.pin = dio_pin;

	 my_set_pin(Tm1638_clk_pin);
	 my_set_pin(Tm1638_dio_pin);

	 my_pin_output_open(Tm1638_clk_pin);
	 my_pin_output_open(Tm1638_dio_pin);
}
#else
#error "implement these MCU/library specific functions: Tm1638_construct(), Tm1638_setup()"
#endif
