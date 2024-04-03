#include <app_cli/cli_app.hh>
#include <jsmn/jsmn_iterate.hh>
#include <debug/log.h>
#include <uout/uout_writer.hh>
#include <full_auto/adapter.hh>
#include <full_auto/automatic_timer.hh>

#define logtag  "rv.cli"

extern AutoTimer at;

bool process_objJson(UoutWriter &td, Jsmn_String::Iterator &it) {
  db_loge("test.json_obj", "all_json: <%s>", it.get_json());

  assert(it->type == JSMN_OBJECT);

  using token_handler_fun_type = bool (*)(class UoutWriter &td, Jsmn_String::Iterator &it, int &err);
  static const token_handler_fun_type tok_processRootChilds_funs[] = { //

      [](class UoutWriter &td, Jsmn_String::Iterator &it, int &err) -> bool {
        if (it.keyIsEqual("auto", JSMN_OBJECT)) {
          return at.handle_json(td.sj(), ++it);
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
