/*
 * tm1638.h
 *
 *  Created on: 02.05.2019
 *      Author: bertw
 */

#ifndef TM1638_INCLUDE_TM1638_H_
#define TM1638_INCLUDE_TM1638_H_

#include <stdint.h>
#include <stdbool.h>



#define TM1638_CMD_TYPE_DATA_MANAGEMENT  0x40
#define TM1638_CMD_TYPE_DISPLAY_CONTROL  0x80
#define TM1638_CMD_TYPE_REGISTER_ADDRESS 0xC0

#define TM1638_CMD_DIR_WRITE   0x00
#define TM1638_CMD_DIR_READ    0x02

#define TM1638_DISPLAY_ON  0x08
#define TM1638_DISPLAY_OFF 0x00
#define TM1638_DISPLAY_DIMMER(value) ((value)&0x7)  //value 0..7

#define TM1638_CMD_ADDR_MODE_INCR   0x00
#define TM1638_CMD_ADDR_MODE_SINGLE 0x04

#define TM1638_REG_OUTPUT 0xC0





extern uint32_t Tm1638_clk_port, Tm1638_dio_port;
extern uint16_t Tm1638_clk_pin, Tm1638_dio_pin;

typedef struct {
	uint32_t mStrobePort;
	uint16_t mStrobePin;


} Tm1638;

uint8_t Tm1638_char_to_7s(char c);

Tm1638 *Tm1638_construct(void *memory, uint32_t stb_port, uint16_t stb_pin);

bool Tm1638_write(Tm1638 *obj, const uint8_t *cmd, uint8_t cmd_len,	const uint8_t *data, uint8_t data_len, const uint8_t *regs);
uint32_t Tm1638_read(Tm1638 *obj);
bool Tm1638_put_value(Tm1638 *obj, uint8_t value, uint8_t register_number);
bool Tm1638_put_char(Tm1638 *obj, char c, uint8_t register_number);
bool Tm1638_put_data(Tm1638 *obj, const uint8_t data[],  uint8_t length, const uint8_t registers[]);

#endif /* TM1638_INCLUDE_TM1638_H_ */

