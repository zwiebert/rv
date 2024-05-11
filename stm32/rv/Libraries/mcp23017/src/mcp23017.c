/*
 * mcp23017.cc
 *
 *  Created on: 29.04.2019
 *      Author: bertw
 */
#include "mcp23017.h"
#include "utils_misc/int_macros.h"

#include <libopencm3/stm32/i2c.h>
#include <stdbool.h>
#include <stdint.h>
#include <string.h>

typedef enum {
	IODIR, IPOL, GPINTEN, DEFVAL, INTCON, IOCON, GPPU, INTF, INTCAP, GPIO, OLAT,
} mcpreg_T;

#define BANK 0

#define bregA(reg, bank) ((bank) ? (reg) : (reg*2))
#define bregB(reg, bank) ((bank) ? (0x10|(reg)) : ((reg*2)+1))

#define regA(reg) bregA((reg),BANK)
#define regB(reg) bregB((reg),BANK)


static bool Mcp23017_writeIO(Mcp23017 *obj, uint8_t start_reg, uint8_t byte_count) {
	int i;
	uint32_t reg32 __attribute__((unused));
	uint32_t i2c = obj->mI2c;

	/* Send START condition. */
	i2c_send_start(i2c);

	/* Waiting for START is send and therefore switched to master mode. */
	while (!((I2C_SR1(i2c) & I2C_SR1_SB)
			& (I2C_SR2(i2c) & (I2C_SR2_MSL | I2C_SR2_BUSY))))
		;

    /* Say to what address we want to talk to. */
    i2c_send_7bit_address(i2c, obj->mAddr, I2C_WRITE);

    /* Waiting for address is transferred. */
    for (int i = 0;; ++i) {
      //if (i > 5000) return false;
      if ((I2C_SR1(i2c) & I2C_SR1_ADDR))
        break;
    }

	/* Cleaning ADDR condition sequence. */
	reg32 = I2C_SR2(i2c);

	/* Sending the data. */
	i2c_send_data(i2c, start_reg);
	while (!(I2C_SR1(i2c) & I2C_SR1_BTF))
		;
	for (i = 0; i < byte_count; ++i) {
		bool last_byte = (i+1) == byte_count;
		uint16_t data_word = obj->mRegs[(start_reg + i) >> 1];
		uint8_t data_byte =
				((start_reg + i) & 1) ? GET_HIGH_BYTE(data_word) : GET_LOW_BYTE(data_word);

		i2c_send_data(i2c, data_byte);
		while (!(I2C_SR1(i2c) & (I2C_SR1_BTF | (last_byte ? I2C_SR1_TxE : 0))))
			;
	}

	/* Send STOP condition. */
	i2c_send_stop(i2c);

	return true;
}

static bool Mcp23017_writeAllRegs(Mcp23017 *obj) {
	return Mcp23017_writeIO(obj, regA(IODIR), 2 * (GPPU + 1 - IODIR))
			&& Mcp23017_writeIO(obj, regA(OLAT), 2);
}

Mcp23017 *Mcp23017_construct( // setup object
		void *memory, // uninitialized object (sizeof Mcp23017)
		uint32_t i2c, // the I2C port handle of libopencm3
		uint8_t slaveAddress, // slave address generated by macro Mcp23017_SLAVE_ADDRESS
		uint16_t gpio_direction, // 1=input
		uint16_t gpo_values, // initial values of output pins
		uint16_t gpi_polarity, // 1=inverse
		uint16_t gpi_pullup, // 1=100kOhm pull-up resistor
		uint16_t gpi_interrupt_enable, // 1=enable
		uint16_t gpi_default_values, // set default values to compare for generating input interrupts
		uint16_t gpi_use_default_values, // 1=default_values, 0=previous_state
		bool intpin_mirror, // IntA/IntB pins are 1=connected or 0=separate for PortA/PortB
		bool intpin_open_drain, //IntA/B pins are true=OpenDrain, false=PushPull (with polarity set by next parameter)
		bool intpin_reverse_output_polarity // true=Active-High, false=Active-Low
		){

	Mcp23017 *obj = memory;
	memset(obj, 0, sizeof(*obj));

	obj->mAddr = slaveAddress;
	obj->mI2c = i2c;

	uint16_t *r = obj->mRegs;
	r[IODIR] = gpio_direction;
	r[IPOL] = gpi_polarity;
	r[GPINTEN] = gpi_interrupt_enable;
	r[DEFVAL] = gpi_default_values;
	r[INTCON] = gpi_use_default_values;
	uint8_t iocon = 0;
	if (intpin_mirror) SET_BIT(iocon, 6);
	if (intpin_open_drain) SET_BIT(iocon, 2);
	if (intpin_reverse_output_polarity) SET_BIT(iocon, 1);
	if (BANK) SET_BIT(iocon, 7);
	PUT_LOW_BYTE(r[IOCON],iocon);
	PUT_HIGH_BYTE(r[IOCON],iocon);

	r[GPPU] = gpi_pullup;
	r[OLAT] = gpo_values;

	Mcp23017_writeAllRegs(obj);
	return memory;
}

Mcp23017 *Mcp23017_construct_out(// setup object with all pins as output
		void *memory, // uninitialized object (sizeof Mcp23017)
		uint32_t i2c, // the I2C port handle of libopencm3
		uint8_t slaveAddress, // slave address generated by macro Mcp23017_SLAVE_ADDRESS
		uint16_t gpo_values // initial values of output pins
		) {
	Mcp23017 *obj = memory;
	memset(obj, 0, sizeof(*obj));

	obj->mAddr = slaveAddress;
	obj->mI2c = i2c;

	uint16_t *r = obj->mRegs;
	r[IODIR] = 0;
	r[OLAT] = gpo_values;

	Mcp23017_writeAllRegs(obj);
	return memory;
}

void Mcp23017_destruct(Mcp23017 *) {

}

bool Mcp23017_putBit(Mcp23017 *obj, uint8_t bitNumber, bool value) {
	bool oldValue = GET_BIT(obj->mRegs[OLAT], bitNumber);
	if (oldValue != value) {
		PUT_BIT(obj->mRegs[OLAT], bitNumber, value);
		return Mcp23017_writeIO(obj, bitNumber < 8 ? regA(OLAT) : regB(OLAT), 1);
	}

	return true;

}

bool Mcp23017_getBit(Mcp23017 *obj, uint8_t bitNumber, bool cached) {
	if (!cached) {

	}

	if (GET_BIT(obj->mRegs[IODIR], bitNumber))
		return GET_BIT(obj->mRegs[GPIO], bitNumber) != 0;
	else
		return GET_BIT(obj->mRegs[OLAT], bitNumber) != 0;
}

bool Mcp23017_putBits(Mcp23017 *obj, uint16_t bitMask, uint16_t values) {
	uint16_t oldReg = obj->mRegs[OLAT];
	uint16_t newReg = (oldReg & ~bitMask) | (values & bitMask);

	if (oldReg != newReg) {
		obj->mRegs[OLAT] = newReg;
		return Mcp23017_writeIO(obj, regA(OLAT), 2);
	}

	return true;
}

uint16_t Mcp23017_getBits(Mcp23017 *obj, uint16_t bitMask, bool cached) {
	if (!cached) {

	}
	return ((obj->mRegs[IODIR] & obj->mRegs[GPIO]) | (~obj->mRegs[IODIR] & obj->mRegs[OLAT])) & bitMask;
	//return obj->mRegs[OLAT] & bitMask; // XXX FIXME
}

bool Mcp23017_forceWrite(Mcp23017 *) {


	return true;
}

bool Mcp23017_statusOk(Mcp23017 *) {
	return true;
}

