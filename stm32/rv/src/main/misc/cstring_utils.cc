#include "cstring_utils.hh"
#include <string.h>
#include <malloc.h>


bool csu_assign(char **dst, const char *src) {
  if (*dst && strcmp(*dst, src) == 0)
    return true;

  size_t src_size = strlen(src) + 1;

  if (src_size == 1) {
    free(*dst);
    *dst = 0;
    return true;
  }

  if (char *ptr = static_cast<char*>(realloc(*dst, src_size))) {
    *dst = strcpy(ptr, src);
    return true;
  }

  return false;
}

void csu_delete(char *p) {
  free(p);
}


void csu_destroy(char **p) {
  csu_delete(*p);
  *p = nullptr;
}



