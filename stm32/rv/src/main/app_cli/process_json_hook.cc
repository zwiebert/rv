

#include <debug/log.h>
#ifdef CONFIG_CLI_DEBUG
#define DEBUG
#define D(x) x
#define L(x) x
#else
#define D(x)
#define L(x) x
#endif
#define logtag "cli"

bool process_json(char *json) {
  D(db_logi(logtag, "%s(%s)", __func__, json));

  return false;
}
