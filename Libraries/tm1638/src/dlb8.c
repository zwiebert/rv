/*
 * l8d8k8.c
 *
 *  Created on: 02.05.2019
 *      Author: bertw
 */

#ifndef TM1638_SRC_L8D8K8_C_
#define TM1638_SRC_L8D8K8_C_

#include "../include/boards/dlb8.h"


int8_t dlb8_get_button(Tm1638 *obj) {
	uint32_t data = Tm1638_read(obj);
	if (!data)
		return -1;

	for(int i=0; i < 8; (++i), (data >>= 4)) {
		if (data & 1) {
			if (i&1) {
				return (i>>1) + 4;
			} else {
				return (i>>1);
			}
		}
	}
	return -1;
}

#endif /* TM1638_SRC_L8D8K8_C_ */
