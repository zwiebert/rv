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
#include "app_config/proj_app_cfg.h"
#include "freertos/FreeRTOS.h"
#include "freertos/task.h"
#include "driver/uart.h"
#include "driver/gpio.h"
#include "debug/debug.h"

#ifndef STM32_BOOT_PIN_REVERSE
#define STM32_SET_BOOT_PIN(high) GPIO_OUTPUT_SET(STM32_BOOT_PIN, (high));
#else
#define STM32_SET_BOOT_PIN(high) GPIO_OUTPUT_SET(STM32_BOOT_PIN, !(high));
#endif

#define D(x) x

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

#define RESET_PIN_MS 1000

typedef enum { STM32_MODE_NONE, STM32_MODE_FIRMWARE, STM32_MODE_BOOTLOADER } stm32_mode_T;

static stm32_mode_T stm32_mode = STM32_MODE_NONE;

bool stm32_isFirmwareRunning(void) { return stm32_mode == STM32_MODE_FIRMWARE; }

void stm32_reset() {
    ets_printf("reboot stm32\n");
    GPIO_OUTPUT_SET(STM32_RESET_PIN, 0);
    vTaskDelay(RESET_PIN_MS / portTICK_PERIOD_MS);
    GPIO_OUTPUT_SET(STM32_RESET_PIN, 1);
}

int stm32_write(const char *data, unsigned data_len) {
	return   uart_write_bytes(UART_NUM_1, (const char *) data, data_len);
}

int stm32_read(char *buf, unsigned buf_size) {
	 return uart_read_bytes(UART_NUM_1, (u8 *)buf, buf_size, 20 / portTICK_RATE_MS);
}


static void stm32_configSerial(stm32_mode_T mode) {
  esp_err_t err;
  if (mode == stm32_mode) {
    D(db_printf("uart1: skip config\n"));
    return; // do nothing if mode stays the same
  }
  if (stm32_mode != STM32_MODE_NONE) {
    D(db_printf("uart1: delete driver\n"));
    uart_driver_delete(UART_NUM_1);
  }

  stm32_mode = mode;

  if (stm32_mode == STM32_MODE_NONE) {
    D(db_printf("uart1: disabled\n"));
    return;
  }

  /* Configure parameters of an UART driver,
   * communication pins and install the driver */
  uart_config_t uart_config = { .baud_rate = 115200, .data_bits = UART_DATA_8_BITS, .parity = UART_PARITY_DISABLE, .stop_bits = UART_STOP_BITS_1, .flow_ctrl =
      UART_HW_FLOWCTRL_DISABLE, };

  if (stm32_mode == STM32_MODE_BOOTLOADER) {
    uart_config.parity = UART_PARITY_EVEN;
    D(db_printf("uart1: even parity\n"));
  }

  err = uart_param_config(UART_NUM_1, &uart_config);
  if (!err) {
    err = uart_set_pin(UART_NUM_1, STM32_UART_TX_PIN, STM32_UART_RX_PIN, ECHO_TEST_RTS, ECHO_TEST_CTS);
    if (!err) {
      err = uart_driver_install(UART_NUM_1, RX_BUF_SIZE, TX_BUF_SIZE, 0, NULL, 0);
      if (!err) {
        D(db_printf("uart1: driver loaded\n"));
        return;
      }
    }
  }
  D(db_printf("uart1: setup failed: err=%d\n", err));
  return;
}

void stm32_runBootLoader() {
  STM32_SET_BOOT_PIN(1);
  stm32_reset();
  stm32_configSerial(STM32_MODE_BOOTLOADER);
}

void stm32_runFirmware() {
  STM32_SET_BOOT_PIN(0);
  stm32_reset();
  stm32_configSerial(STM32_MODE_FIRMWARE);
}


void stm32_setup()
{

  gpio_pad_select_gpio(STM32_RESET_PIN);
  GPIO_OUTPUT_SET(STM32_RESET_PIN, 1);
  gpio_set_direction(STM32_RESET_PIN, GPIO_MODE_OUTPUT_OD);
  GPIO_OUTPUT_SET(STM32_RESET_PIN, 1);

  gpio_pad_select_gpio(STM32_BOOT_PIN);
  STM32_SET_BOOT_PIN(0);
  gpio_set_direction(STM32_BOOT_PIN, GPIO_MODE_OUTPUT_OD);
  STM32_SET_BOOT_PIN(0);

  stm32_configSerial(STM32_MODE_FIRMWARE);

   //xTaskCreate(echo_task, "uart_echo_task", 1024, NULL, 10, NULL);
}
