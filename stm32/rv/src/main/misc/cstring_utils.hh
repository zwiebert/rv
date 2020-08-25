#pragma once

#include <stdbool.h>

#ifdef __cplusplus
extern "C" {
#endif

bool csu_assign(char **dst, const char *src);

#ifdef __cplusplus
}
#endif
