#include "uart.h"

void uart_setup(){
  uart1_setup();
#if defined CONFIG_USART_STDERR && CONFIG_USART_STDERR == 2
  uart2_setup();
#endif
}
