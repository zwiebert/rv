#include <debug/dbg.h>
#include "peri/uart.h"

#include <stdio.h>
#include <stdarg.h>

#define TRACE_MARKER "trace:"
#define TRACE_MARKER_LEN (sizeof TRACE_MARKER - 1)

void db_print(const char *msg) {
  char buf[120];
  snprintf(buf, 120 - 1, "trace: %s\n", msg);
  esp32_puts(buf);
}

void db_printf(char *format, ...) {
  va_list args;
  char buf[120] = TRACE_MARKER;

  va_start(args, format);
  vsnprintf(buf + TRACE_MARKER_LEN, sizeof buf - TRACE_MARKER_LEN - 1, format, args);
  va_end(args);

  esp32_puts(buf);
}
