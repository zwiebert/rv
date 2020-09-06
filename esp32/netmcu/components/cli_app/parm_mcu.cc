#include "app_config/proj_app_cfg.h"

#include <string.h>


#include "txtio/inout.h"
#include "userio_app/status_output.h"
#include "app/rtc.h"
#include "cli_imp.h"
#include "cli/cli.h"
#include "cli_app/cli_app.h"
#include "stm32/stm32.h"
#include "stm32/stm32_bl.h"
#include "stm32/stm32_ota.h"
#include "app/ota.h"
#include "net/http_client.h"


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


int
process_parmMcu(clpar p[], int len) {
  int arg_idx;
  int error_count = 0;

  so_output_message(SO_MCU_begin, NULL);

  for (arg_idx = 1; arg_idx < len; ++arg_idx) {
    const char *key = p[arg_idx].key, *val = p[arg_idx].val;

    if (key == NULL || val == NULL) {
      return -1;
    } else if (strcmp(key, KEY_BOOT_COUNT) == 0 && *val == '?') {
      so_output_message(SO_MCU_BOOT_COUNT, 0);
    } else if (strcmp(key, "rbl") == 0) {
       ets_printf("run bootloader\n");
       stm32_runBootLoader();
    } else if (strcmp(key, "blstart") == 0) {
      stm32Bl_doStart();
    } else if (strcmp(key, "blgid") == 0) {
      stm32Bl_getId();
    } else if (strcmp(key, "blget") == 0) {
      stm32Bl_get();
    } else if (strcmp(key, "rfw") == 0) {
       ets_printf("run firmware\n");
       stm32_runFirmware();
    } else if (strcmp(key, "dlrvbin") == 0) {
      ets_printf("download rv.bin\n");
      stm32Ota_firmwareDownload(val, STM32_FW_FILE_NAME);
      // mcu dlrvbin=http://192.168.1.70:8000/rv.bin;
    } else if (strcmp(key, "rvota") == 0) {
      ets_printf("download rv.bin\n");
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
        so_output_message(SO_MCU_STM32OTA_STATE, 0);
#ifdef STM32OTA_FWURL_MASTER
      } else if (strcmp(val, "github-master") == 0) {
        so_output_message(SO_MCU_STM32OTA, STM32OTA_FWURL_MASTER);
        stm32ota_doUpdate(STM32OTA_FWURL_MASTER);
#endif
#ifdef STM32OTA_FWURL_BETA
      } else if (strcmp(val, "github-beta") == 0) {
        so_output_message(SO_STM32MCU_OTA, STM32OTA_FWURL_BETA);
        stm32ota_doUpdate(STM32OTA_FWURL_BETA);
#endif
      } else {
#ifdef DISTRIBUTION
        ets_printf("forbidden: ota update from given URL\n");
#else
        ets_printf("doing ota update from given URL\n");
        stm32ota_doUpdate(val);
#endif
      }
#endif
#ifdef USE_OTA
    } else if (strcmp(key, "ota") == 0) {
      if (*val == '?') {
        so_output_message(SO_MCU_OTA_STATE, 0);
      } else if (strcmp(val, "github-master") == 0) {
        so_output_message(SO_MCU_OTA, OTA_FWURL_MASTER);
        app_doFirmwareUpdate(OTA_FWURL_MASTER);
      } else if (strcmp(val, "github-beta") == 0) {
        so_output_message(SO_MCU_OTA, OTA_FWURL_BETA);
        app_doFirmwareUpdate(OTA_FWURL_BETA);
      } else {
#ifdef DISTRIBUTION
        ets_printf("forbidden: ota update from given URL\n");
#else
        ets_printf("doing ota update from given URL\n");
        app_doFirmwareUpdate(val);
#endif
      }
#endif
    } else if (strcmp(key, "flrvbin") == 0) {
       ets_printf("flash rv.bin\n");
       stm32Bl_writeMemoryFromBinFile("/spiffs/rv.bin", 0x8000000);
    } else if (strcmp(key, "flrv") == 0) {
      stm32Ota_firmwareUpdate(STM32_FW_FILE_NAME);

#ifdef CONFIG_GPIO_SIZE
    } else if (strncmp(key, "gpio", 4) == 0) {
      int gpio_number = atoi(key + 4);
      mcu_pin_state ps = 0, ps_result = 0;

      if (!is_gpio_number_usable(gpio_number, true)) {
        reply_message("gpio:error", "gpio number cannot be used");
        return -1;
      } else {

        const char *error = NULL;
        for (ps = 0; pin_state_args[ps] != 0; ++ps) {
          if (pin_state_args[ps] == *val) {
            break;
          }
        }

        switch (ps) {

          case PIN_CLEAR:
          case PIN_SET:
          case PIN_TOGGLE:
          error = mcu_access_pin(gpio_number, NULL, ps);
          break;

          case PIN_READ:
          error = mcu_access_pin(gpio_number, &ps_result, ps);
          if (!error) {
            cli_out_mcu_reply_entry(key, (ps_result == PIN_SET ? "1" : "0"), 0);
          }
          break;

          default:
          error = "invalid command";

        }

        if (error) {
          reply_message("gpio:failure", error);
          return -1;
        }
      }
#endif

    } else if (strcmp(key, "up-time") == 0) {
      if (*val == '?') {
        so_output_message(SO_MCU_RUN_TIME, NULL);
      } else {
        reply_message("error:mcu:up-time", "option is read-only");
      }

    } else if (strcmp(key, "version") == 0) {
      so_output_message(SO_MCU_VERSION, NULL);
    } else {
      cli_warning_optionUnknown(key);
    }

  }

  so_output_message(error_count ? SO_STATUS_ERROR : SO_STATUS_OK, 0);
  so_output_message(SO_MCU_end, NULL);

  return 0;
}
