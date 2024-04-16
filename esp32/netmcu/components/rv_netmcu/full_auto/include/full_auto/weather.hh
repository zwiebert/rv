#pragma once

#include <net_http_server/content.hh>
#include "weather/weather_irrigation.hh"
#include "weather/weather_provider_owm.hh"
#include <full_auto/automatic_timer.hh>

class FullAuto {

public:
  FullAuto(Weather_Provider *wp) :
      m_wi(wp), m_at(&m_wi) {
    m_wi.load_past_weather_data();
    m_at.restore_settings();
  }

public:
  Weather_Irrigation &weather_irrigation() { return m_wi; }
  AutoTimer &auto_timer() { return m_at; }

private:
  Weather_Irrigation m_wi;
  AutoTimer m_at;
};

void fa_loop();

/**
 * \brief  call this once at each full hour (+-5min).
 *
 */
bool fa_poll_weather_full_hour();
