#pragma once
#include <app_uout/so_types.h>

void soMsg_fw_start_msg_print(const struct TargetDesc &td);
void soMsg_status_ok(const struct TargetDesc &td);
void soMsg_status_error(const struct TargetDesc &td);
void soMsg_mcu_begin(const struct TargetDesc &td);
void soMsg_mcu_run_time(const struct TargetDesc &td);
void soMsg_mcu_version(const struct TargetDesc &td);
void soMsg_mcu_ota(const struct TargetDesc &td, const char *url);
void soMsg_mcu_ota_state(const struct TargetDesc &td);
void soMsg_mcu_boot_count(const struct TargetDesc &td);
void soMsg_mcu_end(const struct TargetDesc &td);
void soMsg_sep_enable(const struct TargetDesc &td);
void soMsg_sep_disable(const struct TargetDesc &td);
void soMsg_sep_button_pressed_error(const struct TargetDesc &td);
void soMsg_cfgpasswd_ok(const struct TargetDesc &td);
void soMsg_cfgpasswd_wrong(const struct TargetDesc &td);
void soMsg_cfgpasswd_missing(const struct TargetDesc &td);
void soMsg_cuas_start(const struct TargetDesc &td, uint16_t id);
void soMsg_cuas_timeout(const struct TargetDesc &td);
void soMsg_cuas_done(const struct TargetDesc &td);
void soMsg_cuas_state(const struct TargetDesc &td, int state);
void soMsg_pras_start_listening(const struct TargetDesc &td, uint16_t id);
void soMsg_pras_stop_listening(const struct TargetDesc &td);
void soMsg_pras_timeout(const struct TargetDesc &td);
void soMsg_pras_done(const struct TargetDesc &td, bool success, bool unpair);
int soMsg_timer_print_begin(const struct TargetDesc &td);
void soMsg_timer_print_end(const struct TargetDesc &td);
void soMsg_timer_end(const struct TargetDesc &td);



void soMsg_pos_begin(const struct TargetDesc &td);
void soMsg_pos_end(const struct TargetDesc &td);
void soMsg_pair_begin(const struct TargetDesc &td);
void soMsg_pair_end(const struct TargetDesc &td);
void soMsg_pair_all_begin(const struct TargetDesc &td);
void soMsg_pair_all_end(const struct TargetDesc &td);

void soMsg_inet_print_address(const struct TargetDesc &td);
void soMsg_gpio_pin(const struct TargetDesc &td, const so_arg_pch_t a, bool broadcast = false);


void soMsg_print_kvd(const struct TargetDesc &td, const so_arg_kvd_t a);
void soMsg_print_kvs(const struct TargetDesc &td, so_arg_kvs_t a);

void soMsg_kv(const struct TargetDesc &td, const char *key, const char *val);
void soMsg_kv(const struct TargetDesc &td, const char *key, int val);
void soMsg_kv(const struct TargetDesc &td, const char *key, bool val);




void soMsg_FW_START_MSG_PRINT(const struct TargetDesc &td);
void soMsg_STATUS_OK(const struct TargetDesc &td);
void soMsg_STATUS_ERROR(const struct TargetDesc &td);
void soMsg_MCU_begin(const struct TargetDesc &td);
void soMsg_MCU_RUN_TIME(const struct TargetDesc &td);
void soMsg_MCU_VERSION(const struct TargetDesc &td);
void soMsg_MCU_OTA(const struct TargetDesc &td, const char *url);
void soMsg_MCU_OTA_STATE(const struct TargetDesc &td);
void soMsg_MCU_STM32OTA(const struct TargetDesc &td, const char *url);
void soMsg_MCU_STM32OTA_STATE(const struct TargetDesc &td);
void soMsg_MCU_BOOT_COUNT(const struct TargetDesc &td);
void soMsg_MCU_end(const struct TargetDesc &td);
void soMsg_KVS_begin(const struct TargetDesc &td);
void soMsg_KVS_ZN_SINGLE(const struct TargetDesc &td, const char *key);
void soMsg_KVS_ZN_ALL(const struct TargetDesc &td, const char *keyBase);
void soMsg_KVS_end(const struct TargetDesc &td);
void soMsg_RVE_begin(const struct TargetDesc &td);
void soMsg_RVE_PUMP(const struct TargetDesc &td, const so_arg_on_t *state);
void soMsg_RVE_RAIN(const struct TargetDesc &td, const so_arg_on_t *state);
void soMsg_RVE_PRESS_CTL(const struct TargetDesc &td, const so_arg_on_t *state);
void soMsg_RVE_VALVES(const struct TargetDesc &td, const so_arg_valves_t *valves);
void soMsg_RVE_end(const struct TargetDesc &td);
void soMsg_CFGPASSWD_OK(const struct TargetDesc &td);
void soMsg_CFGPASSWD_WRONG(const struct TargetDesc &td);
void soMsg_CFGPASSWD_MISSING(const struct TargetDesc &td);
void soMsg_cfgpasswd_ok(const struct TargetDesc &td);
void soMsg_cfgpasswd_wrong(const struct TargetDesc &td);
void soMsg_cfgpasswd_missing(const struct TargetDesc &td);
void soMsg_INET_PRINT_ADDRESS(const struct TargetDesc &td);
void soMsg_PBUF_begin(const struct TargetDesc &td);
void soMsg_PBUF_KV64(const struct TargetDesc &td, const so_arg_pbuf_t *pba);
void soMsg_PBUF_end(const struct TargetDesc &td);
