/**
 * \file app_settings/app_settings.hh
 * \brief Settings for application components
 */
#include "app_settings/app_settings.hh"
#include <app_config/options.hh>
#include <app_uout/so_config.h>
#include <utils_misc/int_macros.h>
#include "config_kvs/config.h"
#include "config_kvs/comp_settings.hh"
#include <config_kvs/settings.hh>
#include <assert.h>


class AppSettings final: public Settings<configAppItem, CBA_size - CB_size, CB_size> {
public:
  using Base = Settings<configAppItem, CBA_size - CB_size, CB_size>;
  using Item = configAppItem;
  using storeFunT = void (*)(configAppItem item, const char *val);
public:
  constexpr AppSettings() {
    initField(CB_CFG_PASSWD, "C_CFG_PW", otok::NONE, CBT_str, 0, STF_direct);
    initField(CB_TZ, "C_TZ", otok::k_tz, CBT_str, soCfg_TZ, STF_direct);
#ifdef CONFIG_APP_USE_NETWORK
    initField(CB_NETWORK_CONNECTION, "C_NW_CONN", otok::k_network, CBT_i8, soCfg_NETWORK);
#endif
    initField(CB_STM32_INV_BOOTPIN, "C_STM_INV_BP", otok::k_stm32_bootgpio_inv, CBT_i8, soCfg_STM32_BOOTGPIO_INV, STF_direct);
#ifdef CONFIG_APP_USE_LPH
    initField(CB_LPH, "C_LPH", otok::NONE, CBT_blob, nullptr);
#endif
  }

};

static constexpr AppSettings app_settings_obj;
const SettingsBase<configAppItem> &app_sett = app_settings_obj;

