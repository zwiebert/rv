/**
 * \file   debug/dbg.h
 * \brief  Some debug helper macros.
 */

#ifdef __cplusplus
extern "C" {
#endif

void db_print(const char *msg);
void db_printf(const char *format, ...);
#include <assert.h>

#define precond assert
#define postcond assert

#ifdef __cplusplus
}
#endif
