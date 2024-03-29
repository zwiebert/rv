#include <app_cli/cli_app.hh>
#include <jsmn/jsmn_iterate.hh>
#include <debug/log.h>

#include <full_auto/adapter.hh>
#include <full_auto/automatic_timer.hh>

#define logtag  "rv.cli"

#define adapter_prefix  "adapter."
#define zone_prefix  "zone."

extern AutoTimer at;

bool process_objJson(class UoutWriter &td, Jsmn_String::Iterator &it) {
  db_loge("test.json_obj", "all_json: <%s>", it.get_json());

  assert(it->type == JSMN_OBJECT);

  using token_handler_fun_type = bool (*)(class UoutWriter &td, Jsmn_String::Iterator &it, int &err);
  static const token_handler_fun_type tok_processRootChilds_funs[] = { //

      [](class UoutWriter &td, Jsmn_String::Iterator &it, int &err) -> bool {
        if (it.keyIsEqual("auto", JSMN_OBJECT)) {
          auto count = it[1].size;
          for (it += 2; count > 0 && it; --count) {

            if (it.keyStartsWith(zone_prefix, JSMN_OBJECT)) {
              MagValve zone;
              int zone_idx = -1;
              char key[16];
              if (it.getValue(key)) {
                zone_idx = atoi(key + strlen(zone_prefix));
                if (at.update_zone(zone_idx, ++it)) {
                  continue; // update succeeded
                }
              }
              db_loge(logtag, "Could not update zone %d", zone_idx);
              ++err;
              continue;
            }

            if (it.keyStartsWith(adapter_prefix, JSMN_OBJECT)) {
              WeatherAdapter adapter;
              int adapter_idx = -1;
              char key[16];
              if (it.getValue(key)) {
                adapter_idx = atoi(key + strlen(adapter_prefix));
                if (at.update_adapter(adapter_idx, ++it)) {
                  continue; // update succeeded
                }
              }
              db_loge(logtag, "Could not update adapter %d", adapter_idx);
              ++err;
              continue;
            }

            db_loge(logtag, "unknown key found in json.auto");
            ++err;
            Jsmn_String::skip_key_and_value(it);

          }
          return true;
        }
        return false;

      },
      [](class UoutWriter &td, Jsmn_String::Iterator &it, int &err) -> bool {
        return Jsmn_String::skip_key_and_value(it);
      } };

  int err = 0;
  auto count = it->size;
  for (++it; count > 0 && it; --count) {
    for (auto fun : tok_processRootChilds_funs) {
      if (fun(td, it, err))
        break;
    }

  }
  return !err;
  return false;
}
