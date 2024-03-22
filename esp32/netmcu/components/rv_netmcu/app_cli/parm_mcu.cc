#include "app_config/proj_app_cfg.h"

#include <string.h>


#include "txtio/inout.h"
#include "app_uout/status_output.h"
#include "app_misc/rtc.h"
#include "cli_imp.h"
#include "cli/cli.h"
#include "app_cli/cli_app.h"
#include "stm32/stm32.h"
#include "stm32/stm32_bl.h"
#include "stm32/stm32_ota.h"
#include "app_misc/ota.h"
#include "net/http_client.h"
#include "debug/log.h"
#include <app_uout/so_msg.h>
#include "key_value_store/kvs_wrapper.h"


//FIXME: It makes no sense to use debug print or log functions for CLI feedback (not even visible in telnet)
#define logtag "parm_mcu"

#define KEY_BOOT_COUNT "boot-count"


const char cli_help_parmMcu[] = "print=(rtc|cu|reset-info)\n"
#if ENABLE_SPIFFS
    "spiffs=(format|test)\n"
#endif
#ifdef CONFIG_GPIO_SIZE
    "gpioN=(0|1|t|?) clear, set, toggle or read GPIO pin N\n"
#endif
        "up-time=?\n"
        "version=full\n";



static void kvs_print_keys(const char *name_space);


int
process_parmMcu(clpar p[], int len, class UoutWriter &td) {
  int arg_idx;
  int error_count = 0;

  soMsg_MCU_begin(td);


  for (arg_idx = 1; arg_idx < len; ++arg_idx) {
    const char *key = p[arg_idx].key, *val = p[arg_idx].val;

    if (key == NULL || val == NULL) {
      return -1;
    } else if (strcmp(key, KEY_BOOT_COUNT) == 0 && *val == '?') {
      soMsg_MCU_BOOT_COUNT(td);
    } else if (strcmp(key, "rbl") == 0) {
       db_logi(logtag, "run bootloader");
       stm32_runBootLoader();
    } else if (strcmp(key, "blstart") == 0) {
      stm32Bl_doStart();
    } else if (strcmp(key, "blgid") == 0) {
      stm32Bl_getId();
    } else if (strcmp(key, "blget") == 0) {
      stm32Bl_get();
    } else if (strcmp(key, "rfw") == 0) {
       db_logi(logtag, "run firmware");
       stm32_runFirmware();
    } else if (strcmp(key, "dlrvbin") == 0) {
      db_logi(logtag, "download rv.bin");
      stm32Ota_firmwareDownload(val, STM32_FW_FILE_NAME);
      // mcu dlrvbin=http://192.168.1.70:8000/rv.bin;
    } else if (strcmp(key, "rvota") == 0) {
      db_logi(logtag, "download rv.bin");
      if (stm32Ota_firmwareDownload(val, STM32_FW_FILE_NAME)) {
        if (stm32Ota_firmwareUpdate(STM32_FW_FILE_NAME)) {
        } else {
          ++error_count;
        }
      } else {
        ++error_count;
      }
    } else if (strcmp(key, "rvfl") == 0) {
      stm32Ota_firmwareUpdate(STM32_FW_FILE_NAME);
#ifdef USE_STM32OTA
    } else if (strcmp(key, "stm32ota") == 0) {
      if (*val == '?') {
        soMsg_MCU_STM32OTA_STATE(td);
#ifdef STM32OTA_FWURL_MASTER
      } else if (strcmp(val, "github-master") == 0) {
        soMsg_MCU_STM32OTA(STM32OTA_FWURL_MASTER);
        stm32ota_doUpdate(STM32OTA_FWURL_MASTER);
#endif
#ifdef STM32OTA_FWURL_BETA
      } else if (strcmp(val, "github-beta") == 0) {
        soMsg_STM32MCU_OTA(STM32OTA_FWURL_BETA);
        stm32ota_doUpdate(STM32OTA_FWURL_BETA);
#endif
      } else {
#ifdef DISTRIBUTION
        db_logi(logtag, "forbidden: ota update from given URL");
#else
        db_logi(logtag, "doing ota update from given URL");
        stm32ota_doUpdate(val);
#endif
      }
#endif
#ifdef USE_OTA
    } else if (strcmp(key, "ota") == 0) {
      if (*val == '?') {
        soMsg_MCU_OTA_STATE(td);
      } else if (strcmp(val, "github-master") == 0) {
        soMsg_MCU_OTA(td, OTA_FWURL_MASTER);
        app_doFirmwareUpdate(OTA_FWURL_MASTER);
      } else if (strcmp(val, "github-beta") == 0) {
        soMsg_MCU_OTA(td, OTA_FWURL_BETA);
        app_doFirmwareUpdate(OTA_FWURL_BETA);
      } else {
#ifdef DISTRIBUTION
        db_logi(logtag, "forbidden: ota update from given URL");
#else
        db_logi(logtag, "doing ota update from given URL");
        app_doFirmwareUpdate(val);
#endif
      }
#endif
    } else if (strcmp(key, "flrvbin") == 0) {
       db_logi(logtag, "flash rv.bin");
       stm32Bl_writeMemoryFromBinFile("/spiffs/rv.bin", 0x8000000);
    } else if (strcmp(key, "flrv") == 0) {
      stm32Ota_firmwareUpdate(STM32_FW_FILE_NAME);

    } else if (strcmp(key, "kvs-pk") == 0) {
        kvs_print_keys(val);
        break;


#ifdef ACCESS_GPIO
    } else if (strncmp(key, "gpio", 4) == 0) {
      int gpio_number = atoi(key + 4);


      if (!is_gpio_number_usable(gpio_number, true)) {
        reply_message(td, "gpio:error", "gpio number cannot be used");
        return -1;
      } else {

        const char *error = NULL;
        int psi;
        for (psi = 0; pin_state_args[psi] != 0; ++psi) {
          if (pin_state_args[psi] == *val) {
            break;
          }
        }
        mcu_pin_state ps = (mcu_pin_state)psi, ps_result = PIN_READ;
        switch (ps) {

          case PIN_CLEAR:
          case PIN_SET:
          case PIN_TOGGLE:
          error = mcu_access_pin(gpio_number, NULL, ps);
          break;

          case PIN_READ:
          error = mcu_access_pin(gpio_number, &ps_result, ps);
          if (!error) {
            soMsg_gpio_pin(td, so_arg_pch_t {gpio_number, ps_result});
          }
          break;

          default:
          error = "invalid command";

        }

        if (error) {
          reply_message(td, "gpio:failure", error);
          return -1;
        }
      }
#endif


    } else if (strcmp(key, "up-time") == 0) {
      if (*val == '?') {
        soMsg_MCU_RUN_TIME(td);
      } else {
        reply_message(td, "error:mcu:up-time", "option is read-only");
      }

    } else if (strcmp(key, "version") == 0) {
      soMsg_MCU_VERSION(td);
    } else {
      cli_warning_optionUnknown(td, key);
    }

  }

  soMsg_MCU_end(td);

  return 0;
}


static void kvs_print_keys(const char *name_space) {
  kvs_foreach(name_space, KVS_TYPE_ANY,

  /// match any key name
      [](const char*, int) -> bool {
        return true;
      },0,

      /// print each key
      [](const char *key, kvs_type_t type, void *args) -> kvs_cbrT {
        io_printf("key: %s, type: %d\n", key, (int )type);
        return kvsCb_match;
      }, nullptr);
}
