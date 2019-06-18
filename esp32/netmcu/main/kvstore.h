/*
 * kvstore.h
 *
 *  Created on: 12.06.2019
 *      Author: bertw
 */

#ifndef MAIN_KVSTORE_H_
#define MAIN_KVSTORE_H_

#include <stdbool.h>

bool kvs_store_string(const char *key, const char *val);
bool kvs_get_string(const char *key, char *buf, size_t buf_size);


#endif /* MAIN_KVSTORE_H_ */
