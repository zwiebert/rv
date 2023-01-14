#pragma once
#include <app_uout/so_types.h>

void soMsg_fw_start_msg_print(const class UoutWriter &td);
void soMsg_status_ok(const class UoutWriter &td);
void soMsg_status_error(const class UoutWriter &td);
void soMsg_mcu_begin(const class UoutWriter &td);
void soMsg_mcu_run_time(const class UoutWriter &td);
void soMsg_mcu_version(const class UoutWriter &td);
void soMsg_mcu_ota(const class UoutWriter &td, const char *url);
void soMsg_mcu_ota_state(const class UoutWriter &td);
void soMsg_mcu_boot_count(const class UoutWriter &td);
void soMsg_mcu_end(const class UoutWriter &td);
void soMsg_sep_enable(const class UoutWriter &td);
void soMsg_sep_disable(const class UoutWriter &td);
void soMsg_sep_button_pressed_error(const class UoutWriter &td);
void soMsg_cfgpasswd_ok(const class UoutWriter &td);
void soMsg_cfgpasswd_wrong(const class UoutWriter &td);
void soMsg_cfgpasswd_missing(const class UoutWriter &td);
void soMsg_cuas_start(const class UoutWriter &td, uint16_t id);
void soMsg_cuas_timeout(const class UoutWriter &td);
void soMsg_cuas_done(const class UoutWriter &td);
void soMsg_cuas_state(const class UoutWriter &td, int state);
void soMsg_pras_start_listening(const class UoutWriter &td, uint16_t id);
void soMsg_pras_stop_listening(const class UoutWriter &td);
void soMsg_pras_timeout(const class UoutWriter &td);
void soMsg_pras_done(const class UoutWriter &td, bool success, bool unpair);
int soMsg_timer_print_begin(const class UoutWriter &td);
void soMsg_timer_print_end(const class UoutWriter &td);
void soMsg_timer_end(const class UoutWriter &td);



void soMsg_pos_begin(const class UoutWriter &td);
void soMsg_pos_end(const class UoutWriter &td);
void soMsg_pair_begin(const class UoutWriter &td);
void soMsg_pair_end(const class UoutWriter &td);
void soMsg_pair_all_begin(const class UoutWriter &td);
void soMsg_pair_all_end(const class UoutWriter &td);

void soMsg_inet_print_address(const class UoutWriter &td);
void soMsg_gpio_pin(const class UoutWriter &td, const so_arg_pch_t a, bool broadcast = false);


void soMsg_print_kvd(const class UoutWriter &td, const so_arg_kvd_t a);
void soMsg_print_kvs(const class UoutWriter &td, so_arg_kvs_t a);

void soMsg_kv(const class UoutWriter &td, const char *key, const char *val);
void soMsg_kv(const class UoutWriter &td, const char *key, int val);
void soMsg_kv(const class UoutWriter &td, const char *key, bool val);




void soMsg_FW_START_MSG_PRINT(const class UoutWriter &td);
void soMsg_STATUS_OK(const class UoutWriter &td);
void soMsg_STATUS_ERROR(const class UoutWriter &td);
void soMsg_MCU_begin(const class UoutWriter &td);
void soMsg_MCU_RUN_TIME(const class UoutWriter &td);
void soMsg_MCU_VERSION(const class UoutWriter &td);
void soMsg_MCU_OTA(const class UoutWriter &td, const char *url);
void soMsg_MCU_OTA_STATE(const class UoutWriter &td);
void soMsg_MCU_STM32OTA(const class UoutWriter &td, const char *url);
void soMsg_MCU_STM32OTA_STATE(const class UoutWriter &td);
void soMsg_MCU_BOOT_COUNT(const class UoutWriter &td);
void soMsg_MCU_end(const class UoutWriter &td);
void soMsg_KVS_begin(const class UoutWriter &td);
void soMsg_KVS_ZN_SINGLE(const class UoutWriter &td, const char *key);
void soMsg_KVS_ZN_ALL(const class UoutWriter &td, const char *keyBase);
void soMsg_KVS_end(const class UoutWriter &td);
void soMsg_RVE_begin(const class UoutWriter &td);
void soMsg_RVE_PUMP(const class UoutWriter &td, const so_arg_on_t *state);
void soMsg_RVE_RAIN(const class UoutWriter &td, const so_arg_on_t *state);
void soMsg_RVE_PRESS_CTL(const class UoutWriter &td, const so_arg_on_t *state);
void soMsg_RVE_VALVES(const class UoutWriter &td, const so_arg_valves_t *valves);
void soMsg_RVE_end(const class UoutWriter &td);
void soMsg_CFGPASSWD_OK(const class UoutWriter &td);
void soMsg_CFGPASSWD_WRONG(const class UoutWriter &td);
void soMsg_CFGPASSWD_MISSING(const class UoutWriter &td);
void soMsg_cfgpasswd_ok(const class UoutWriter &td);
void soMsg_cfgpasswd_wrong(const class UoutWriter &td);
void soMsg_cfgpasswd_missing(const class UoutWriter &td);
void soMsg_INET_PRINT_ADDRESS(const class UoutWriter &td);
void soMsg_PBUF_begin(const class UoutWriter &td);
void soMsg_PBUF_KV64(const class UoutWriter &td, const so_arg_pbuf_t *pba);
void soMsg_PBUF_end(const class UoutWriter &td);
