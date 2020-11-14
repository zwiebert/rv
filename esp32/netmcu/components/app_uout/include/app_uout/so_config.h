#pragma once

struct TargetDesc;

void soCfg_all(const struct TargetDesc &td);
void soCfg_all_net(const struct TargetDesc &td);
void soCfg_all_rv(const struct TargetDesc &td);
void soCfg_all_time(const struct TargetDesc &td);


void soCfg_BAUD(const struct TargetDesc &td);
void soCfg_RTC(const struct TargetDesc &td);
void soCfg_CU(const struct TargetDesc &td);
void soCfg_NETWORK(const struct TargetDesc &td);
void soCfg_TZ(const struct TargetDesc &td);
void soCfg_TIMEZONE(const struct TargetDesc &td);
void soCfg_DST(const struct TargetDesc &td);
void soCfg_GPIO_PIN(const struct TargetDesc &td, int gpio_number);
void soCfg_GPIO_MODES(const struct TargetDesc &td);
void soCfg_GPIO_MODES_AS_STRING(const struct TargetDesc &td);

void soCfg_begin(const struct TargetDesc &td);
void soCfg_end(const struct TargetDesc &td);

void soCfg_STM32_BOOTGPIO_INV(const struct TargetDesc &td);
