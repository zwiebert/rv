/*
 * app.cc
 *
 *  Created on: 04.06.2019
 *      Author: bertw
 */
#include "user_config.h"
#include <malloc.h>
#include <stddef.h>


#ifdef USE_MALLOC_IN_NEW_GLOBAL
void* operator new(size_t nbytes) {
  return malloc(nbytes);
}

void* operator new[](size_t nbytes) {
  return malloc(nbytes);
}

void operator delete(void *p) {
  free(p);
}
void operator delete(void *p, size_t ) {
  free(p);
}
#endif



