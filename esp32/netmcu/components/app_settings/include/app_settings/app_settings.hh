/**
 * \file app_settings/app_settings.hh
 * \brief Settings for application components
 */
#pragma once
#include "app_config/proj_app_cfg.h"
#include <app_config/options.hh>
#include <app_uout/so_config.h>
#include <utils_misc/int_macros.h>
#include "config.h"
#include <config_kvs/comp_settings.hh>
#include <config_kvs/settings.hh>
#include <assert.h>

enum configAppItem : i8 {
  CBA_NONE = -1,
  CBA_start = CB_size - 1, //
  //----------------
  CB_CFG_PASSWD,  CB_TZ,
#ifdef USE_NETWORK
  CB_NETWORK_CONNECTION,
#endif
  CB_STM32_INV_BOOTPIN,
#ifdef USE_LPH
  CB_LPH,
#endif

//-----------
  CBA_size
};

class AppSettings: public Settings<configAppItem, CBA_size - CB_size, CB_size> {
public:
  using Base = Settings<configAppItem, CBA_size - CB_size, CB_size>;
  using Item = configAppItem;
  using storeFunT = void (*)(configAppItem item, const char *val);
public:
  constexpr AppSettings() {
    initField(CB_CFG_PASSWD, "C_CFG_PW", otok::NONE, CBT_str, 0, STF_direct);
#ifndef USE_POSIX_TIME
#error
#else
    initField(CB_TZ, "C_TZ", otok::k_tz, CBT_str, soCfg_TZ, STF_direct);
#endif
#ifdef USE_NETWORK
    initField(CB_NETWORK_CONNECTION, "C_NW_CONN", otok::k_network, CBT_i8, soCfg_NETWORK);
#endif
    initField(CB_STM32_INV_BOOTPIN, "C_STM_INV_BP", otok::k_stm32_bootgpio_inv, CBT_i8, soCfg_STM32_BOOTGPIO_INV, STF_direct);
    initField(CB_LPH, "C_LPH", otok::NONE, CBT_blob, nullptr);
  }

};

constexpr AppSettings app_settings;

constexpr const char* settings_get_kvsKey(configAppItem item) {
  return app_settings.get_kvsKey(item);
}
constexpr KvsType settings_get_kvsType(configAppItem item) {
  return app_settings.get_kvsType(item);
}
constexpr otok settings_get_optKey(configAppItem item) {
  return app_settings.get_optKey(item);
}
constexpr const char* settings_get_optKeyStr(configAppItem item) {
  return app_settings.get_optKeyStr(item);
}
constexpr AppSettings::Base::soCfgFunT settings_get_soCfgFun(configAppItem item) {
  return app_settings.get_soCfgFun(item);
}
constexpr AppSettings::Base::soCfgFunT settings_get_soCfgFun(otok opt_key) {
  if (auto res = app_settings.get_soCfgFun(opt_key))
    return res;
  return comp_settings.get_soCfgFun(opt_key);
}
constexpr StoreFun settings_get_storeFunOk(otok opt_key) {
  if (auto res = app_settings.get_storeFun(opt_key))
    return res;
  return comp_settings.get_storeFun(opt_key);
}
bool config_item_modified(enum configAppItem item);
