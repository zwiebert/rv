#pragma once

class UoutWriter;

void soCfg_all(const class UoutWriter &td);
void soCfg_all_net(const class UoutWriter &td);
void soCfg_all_rv(const class UoutWriter &td);
void soCfg_all_time(const class UoutWriter &td);


void soCfg_BAUD(const class UoutWriter &td);
void soCfg_RTC(const class UoutWriter &td);
void soCfg_CU(const class UoutWriter &td);
void soCfg_NETWORK(const class UoutWriter &td);
void soCfg_TZ(const class UoutWriter &td);
void soCfg_TIMEZONE(const class UoutWriter &td);
void soCfg_DST(const class UoutWriter &td);
void soCfg_GPIO_PIN(const class UoutWriter &td, int gpio_number);
void soCfg_GPIO_MODES(const class UoutWriter &td);
void soCfg_GPIO_MODES_AS_STRING(const class UoutWriter &td);

void soCfg_begin(const class UoutWriter &td);
void soCfg_end(const class UoutWriter &td);

void soCfg_STM32_BOOTGPIO_INV(const class UoutWriter &td);
