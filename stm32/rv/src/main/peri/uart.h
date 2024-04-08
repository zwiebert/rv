/**
 * \file   peri/uart.h
 * \brief  Serial port setup and I/O functions.
 */

#include "uart1.h"
#include "uart2.h"

#define esp32_write usart1_write
#define esp32_puts usart1_puts
#define esp32_read usart1_read
#define esp32_getc usart1_getc
#define esp32_putc usart1_putc

void uart_setup(void);
