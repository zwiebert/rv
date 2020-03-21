/*
 * unity_setup.c
 *
 *  Created on: 27.02.2020
 *      Author: bertw
 */

#include "key_value_store/kvs_wrapper.h"
#include "storage/storage.h"
#include "txtio/inout.h"
#include "config/config.h"
#include "cli/mutex.h"

int unit_testing;

void mcu_init(void);

void unity_suiteSetUp() {

  unit_testing = 1;

#if 0
 mcu_init();
#else
  //config_setup();

  mutex_setup();
  kvs_setup();
  txtio_setup();
  stor_setup();
#endif

}
