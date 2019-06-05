/*
 * uart.c
 *
 *  Created on: 15.05.2019
 *      Author: bertw
 */


/* UART Echo Example

   This example code is in the Public Domain (or CC0 licensed, at your option.)

   Unless required by applicable law or agreed to in writing, this
   software is distributed on an "AS IS" BASIS, WITHOUT WARRANTIES OR
   CONDITIONS OF ANY KIND, either express or implied.
*/
#include <stdio.h>
#include "user_config.h"
#include "freertos/FreeRTOS.h"
#include "freertos/task.h"
#include "driver/uart.h"
#include "driver/gpio.h"

/**
 * This is an example which echos any data it receives on UART1 back to the sender,
 * with hardware flow control turned off. It does not use UART driver event queue.
 *
 * - Port: UART1
 * - Receive (Rx) buffer: on
 * - Transmit (Tx) buffer: off
 * - Flow control: off
 * - Event queue: off
 * - Pin assignment: see defines below
 */

#ifndef STM32_UART_TX_PIN
#define STM32_UART_TX_PIN  (GPIO_NUM_4)
#endif
#ifndef STM32_UART_RX_PIN
#define STM32_UART_RX_PIN  (GPIO_NUM_36)
#endif

#define ECHO_TEST_RTS  (UART_PIN_NO_CHANGE)
#define ECHO_TEST_CTS  (UART_PIN_NO_CHANGE)

#define RX_BUF_SIZE 2048
#define TX_BUF_SIZE 0


int stm32_write(const char *data, unsigned data_len) {
	return   uart_write_bytes(UART_NUM_1, (const char *) data, data_len);
}

int stm32_read(char *buf, unsigned buf_size) {
	 return uart_read_bytes(UART_NUM_1, (uint8_t *)buf, buf_size, 20 / portTICK_RATE_MS);
}

void uart_setup()
{
    /* Configure parameters of an UART driver,
     * communication pins and install the driver */
    uart_config_t uart_config = {
        .baud_rate = 115200,
        .data_bits = UART_DATA_8_BITS,
        .parity    = UART_PARITY_DISABLE,
        .stop_bits = UART_STOP_BITS_1,
        .flow_ctrl = UART_HW_FLOWCTRL_DISABLE
    };
    uart_param_config(UART_NUM_1, &uart_config);
    uart_set_pin(UART_NUM_1, STM32_UART_TX_PIN, STM32_UART_RX_PIN, ECHO_TEST_RTS, ECHO_TEST_CTS);
    uart_driver_install(UART_NUM_1, RX_BUF_SIZE, TX_BUF_SIZE, 0, NULL, 0);



   //xTaskCreate(echo_task, "uart_echo_task", 1024, NULL, 10, NULL);
}
