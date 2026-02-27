#pragma once
#include "automatic_timer_data.hh"

namespace app::fa {

class AutoTimer : public AutoTimerData {
public:
  void todo_loop();
};


} // namespace

