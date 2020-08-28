#pragma once
#include <stddef.h>
#ifdef __cplusplus
extern "C" {
#endif

int esp32_write(const char *data, size_t data_len);
int esp32_puts(const char *s);
int esp32_read(char *buf, size_t buf_size);
int esp32_getc(void);


#ifdef __cplusplus
}
#endif

void uart_setup(void);
