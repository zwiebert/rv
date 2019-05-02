/*
 * board_dlb8.h
 * "LED&KEY" module with TM1638 and 8 digits, 8 Mono-LEDs, 8 buttons
 *  Created on: 02.05.2019
 *      Author: bertw
 */
#include "../tm1638.h"

#ifndef TM1638_INCLUDE_BOARD_D8L8K8_H_
#define TM1638_INCLUDE_BOARD_D8L8K8_H_

//
#define LED_KEY_ADDR_7S1(n) (TM1638_REG_OUTPUT + (n)*2)
#define LED_KEY_ADDR_LED(n) (TM1638_REG_OUTPUT + (n)*2 +1)
#define LED_KEY_POS_TO_REG(pos) ((pos)*2)


// bits in output of Tm1638_read() ... not needed for dlb8_get_button()
#define LED_KEY_BUTTON_0 0x00000001
#define LED_KEY_BUTTON_1 0x00000100
#define LED_KEY_BUTTON_2 0x00010000
#define LED_KEY_BUTTON_3 0x01000000
#define LED_KEY_BUTTON_4 0x00000010
#define LED_KEY_BUTTON_5 0x00001000
#define LED_KEY_BUTTON_6 0x00100000
#define LED_KEY_BUTTON_7 0x10000000



int8_t dlb8_get_button(Tm1638 *obj);


#endif /* TM1638_INCLUDE_BOARD_D8L8K8_H_ */
