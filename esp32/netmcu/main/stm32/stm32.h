/*
 * uart.h
 *
 *  Created on: 15.05.2019
 *      Author: bertw
 */

#ifndef MAIN_STM32_H_
#define MAIN_STM32_H_

#include <stdbool.h>

int stm32_write(const char *data, unsigned data_len);
int stm32_read(char *buf, unsigned buf_size);
void stm32_setup(void);

void stm32_runBootLoader(void);
void stm32_runFirmware(void);
void stm32_reset(void);

bool stm32_isFirmwareRunning(void);

#endif /* MAIN_STM32_H_ */
