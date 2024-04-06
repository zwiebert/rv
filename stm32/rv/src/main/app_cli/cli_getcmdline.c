/**
 * \file cli/cli_getcmdline.c
 * \brief  get command-line from from companion MCU
 */
#include "../peri/uart.h"
#include "cli_imp.h"
#include <stdbool.h>
#include <stddef.h>
#include <ctype.h>


#define D(x)

char cmd_buf[CMD_BUF_SIZE];

char *
get_commandline(void) {
  char *result = NULL;

  static int cmd_buf_idx;
  static bool error;
  int c;
  static int quoteCount;

  while ((c = esp32_getc()) != -1) {
	  D(printf("get: %d, (%c)\n", c, (char)c));

	  if (cmd_buf_idx == 0) {
	    if (c == '\n' || c == '\r' || c == ';') {
	      error = false;
	      continue; // skip leading whitespaces
	    }
	  }


    // backspace: remove last character from buffer
    if (c == '\b') {
      if (cmd_buf_idx == 0)
        continue;
      if (cmd_buf[--cmd_buf_idx] == '\"')
        --quoteCount;
      continue;
    }

    // count the quotes, so we know if we are inside or outside a quoted word
    if (c == '\"') {
      ++quoteCount;
    }

    // to throw away rest of a too long command line from earlier
    if (error) {
      if (c == ';')
        error = false;
      continue;
    }
    // to make sure there is at least 1 free byte available in CMD_BUF
    if (!((cmd_buf_idx + 1) < CMD_BUF_SIZE)) {
      goto err;
    }

    // handle special characters, if outside a quoted word
    if ((quoteCount & 1) == 0) {
      // ';' or '\n' is used to terminate a command line //TODO: '\n' not possible because of line 24 above
      if (c == '\n' || (c == ';' && (quoteCount & 1) == 0)) {
        if (cmd_buf_idx == 0)
          goto succ;
        cmd_buf[cmd_buf_idx] = '\0';
        result = cmd_buf;
        goto succ;
      }
    }

    // store char to buffer
    if (iscntrl(c))
      continue;

    cmd_buf[cmd_buf_idx++] = (char) c;
  }

  goto cont;

  err: error = true;
  succ: cmd_buf_idx = 0;
  quoteCount = 0;
  cont: return result;
}
