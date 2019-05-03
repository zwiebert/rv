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
#define LED_KEY_ADDR_DIGIT(n) (TM1638_REG_OUTPUT + (n)*2)
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



// get pressed button
// return value:
//   if no button is pressed: 0
//   if one or more buttons are pressed: bits 0..7 are set representing the pressed buttons 0..7 (or 1..8)
//
uint8_t dlb8_get_buttons(Tm1638 *obj);

// get only pressed buttons which were not pressed at the last call of this function
uint8_t dlb8_get_changed_buttons(Tm1638 *obj);

//  put led on or off
// parameters:
//    mask: bit mask of LEDs to be addressed
//    value: on=true, off=false
// return value:
//    if success: true
//
bool dlb8_put_leds(Tm1638 *obj, uint8_t mask, bool value);

//  put value to digit
// parameters:
//    mask: bit mask of Digits to be addressed
//    value: bits 0..6 are representing digit segments a..g. bit 7 represents decimal point.
// return value:
//    if success: true
//
bool dlb8_put_digits(Tm1638 *obj, uint8_t mask, uint8_t value);

// put char to digit
#define dlb8_put_chars(obj,mask,c, dp) dlb8_put_digits(obj,mask, Tm1638_char_to_7s((c)) | ((dp)?0x80:0))

#endif /* TM1638_INCLUDE_BOARD_D8L8K8_H_ */
