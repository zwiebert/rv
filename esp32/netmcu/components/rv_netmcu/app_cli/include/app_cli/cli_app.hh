/**
 * \file   cli/cli_app.hh
 * \brief  CLI main header file
 */
#include "cli_app.h"
#include <jsmn/jsmn_iterate.hh>

bool process_objJson(class UoutWriter &td, jsoneat::Jsmn_String::Iterator &it);
