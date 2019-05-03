/*
 * l8d8k8.c
 *
 *  Created on: 02.05.2019
 *      Author: bertw
 */

#ifndef TM1638_SRC_L8D8K8_C_
#define TM1638_SRC_L8D8K8_C_

#include "../include/boards/dlb8.h"

uint8_t dlb8_get_changed_buttons(Tm1638 *obj) {

	uint8_t result = dlb8_get_buttons(obj);
	uint8_t changed;

	if ((changed = obj->user_data[0] ^ result)) {
		obj->user_data[0] = result;
		result &= changed;
		return result;
	}
	return 0;
}

uint8_t dlb8_get_buttons(Tm1638 *obj) {
	uint32_t data = Tm1638_read(obj);
	uint8_t result = 0;
	if (!data)
		return 0;

	for(int i=0; i < 8; (++i), (data >>= 4)) {
		if (data & 1) {
			if (i&1) {
				result |= (i>>1) + 4;
			} else {
				result |= (i>>1);
			}
		}
	}



	return result;
}

bool dlb8_put_leds(Tm1638 *obj, uint8_t mask, bool value) {
	uint8_t data[8], addr[8], data_len = 0;

	for (int i=0; i < 8; (++i), (mask >>= 1)) {
		if (mask&1) {
			data[data_len] = value ? 0x03 : 0x00;
			addr[data_len++] =  LED_KEY_ADDR_LED(i);
		}
	}
	return Tm1638_put_data(obj, data, data_len, addr);
}

bool dlb8_put_digits(Tm1638 *obj, uint8_t mask, uint8_t value) {
	uint8_t data[8], addr[8], data_len = 0;

	for (int i=0; i < 8; (++i), (mask >>= 1)) {
		if (mask&1) {
			data[data_len] = value;
			addr[data_len++] =  LED_KEY_ADDR_DIGIT(i);
		}
	}
	return Tm1638_put_data(obj, data, data_len, addr);
}

#endif /* TM1638_SRC_L8D8K8_C_ */
