#pragma once

#include <stdbool.h>

#ifdef __cplusplus
extern "C" {
#endif

bool csu_assign(char **dst, const char *src);
void csu_delete(char *p);
void csu_destroy(char **p);

#ifdef __cplusplus
}
#endif
