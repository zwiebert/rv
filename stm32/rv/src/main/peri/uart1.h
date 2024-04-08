/**
 * \file   peri/uart.h
 * \brief  Serial port setup and I/O functions.
 */

#pragma once
#include <stddef.h>
#ifdef __cplusplus
extern "C" {
#endif

/// \brief  write DATA with length DATA_LEN to USART
int usart1_write(const char *data, size_t data_len);

/// \brief write null terminated string S to USART
int usart1_puts(const char *s);

/// \brief read from USART into BUF with maximal length BUF_SIZE (non-blocking)
int usart1_read(char *buf, size_t buf_size);

/// \brief read character from USART (non-blocking)
int usart1_getc(void);
int usart1_putc(char c);


#ifdef __cplusplus
}
#endif

/// \brief  Set up the USART at startup
void uart1_setup(void);
