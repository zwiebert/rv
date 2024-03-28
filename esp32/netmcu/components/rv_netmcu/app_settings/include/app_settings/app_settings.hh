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
#include <config_kvs/settings_template.hh>
#include <assert.h>

enum configAppItem : i8 {
  CBA_NONE = -1,
  CBA_start = CB_size - 1, //
  //----------------
  CB_CFG_PASSWD,  CB_TZ,
#ifdef CONFIG_APP_USE_NETWORK
  CB_NETWORK_CONNECTION,
#endif
#ifdef CONFIG_APP_USE_LPH
  CB_LPH,
#endif

//-----------
  CBA_size
};

extern const SettingsBase<configAppItem> &app_sett;

bool config_item_modified(enum configAppItem item);

/// overloaded function for template usage
constexpr auto settings_get_kvsKey(configAppItem item) {
  return app_sett.get_kvsKey(item);
}
/// overloaded function for template usage
constexpr auto settings_get_kvsType(configAppItem item) {
  return app_sett.get_kvsType(item);
}
/// overloaded function for template usage
constexpr auto settings_get_optKey(configAppItem item) {
  return app_sett.get_optKey(item);
}
/// overloaded function for template usage
constexpr auto settings_get_optKeyStr(configAppItem item) {
  return app_sett.get_optKeyStr(item);
}
