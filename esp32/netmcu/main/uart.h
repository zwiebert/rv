/*
 * uart.h
 *
 *  Created on: 15.05.2019
 *      Author: bertw
 */

#ifndef MAIN_UART_H_
#define MAIN_UART_H_

int stm32_write(const char *data, unsigned data_len);
int stm32_read(char *buf, unsigned buf_size);
void uart_setup(void);

#endif /* MAIN_UART_H_ */
