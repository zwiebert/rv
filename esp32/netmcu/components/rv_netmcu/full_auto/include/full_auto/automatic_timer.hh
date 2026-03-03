#pragma once
#include "automatic_timer_data.hh"

namespace app::fa {

class AutoTimer : public AutoTimerData {
public:
  void todo_loop();
  /**
   * \brief       Test if the given zone is scheduled for now or a given future time
   * \param v     the zone in question
   * \param twhen the time for which we ask. Should be now or in the future.
   * \return      true, if zone is due
   */
  bool should_zone_be_due(const IrrigationZone &z, const time_t twhen = time(0)) const;

};


} // namespace

