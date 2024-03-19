#pragma once

#include <weather/weather_data.hh>
#include <weather/weather_irrigation.hh>
#include "jsmn/jsmn_iterate.hh"
#include <string>

struct location_data {

};
/***
 * \brief Adapt Irrigation according to weather data
 */
struct WeatherAdapter : public Weather_Adapter_Neutral {
  using self_type = WeatherAdapter;

  WeatherAdapter() :
      d_temp(0.06), d_wind(0.001), d_humi(0.01), d_clouds(0.01) {
  }

  int modify_interval(const weather_data &wd, unsigned interval) const {
    auto f = get_factor(wd);
    auto result = static_cast<int>(interval * f);
    if (result < 0)
      return 0;
    return result;
  }

  float get_factor(const weather_data &wd) const override {
    if (!flags.exists)
      return 1.0; // return neutral factor

    auto &rva = *this;
    const float percent_factor = 0.00000001;  // to avoid getting the actual factors too small for JSON
    float f_temp = -((wd.get_temp_celsius() - 20) * rva.d_temp);
    float f_wind = -((wd.get_wind_speed_kph()) * rva.d_wind);
    float f_humi = +((wd.get_relative_humidity_percent() - 50) * rva.d_humi * percent_factor);
    float f_clouds = +((wd.get_cloud_coverage_percent() - 50) * rva.d_clouds * percent_factor);

    float f = 1.0 + f_temp + f_wind + f_humi + f_clouds;

    return f;
  }



public:
  int to_json(char *dst, size_t dst_size) const {
    auto n = snprintf(dst, dst_size, //
        R"({"name":"%s","flags":{"exists":%d,"neutral":%d,"read_only":%d},"temp":%g,"wind":%g,"humi":%g,"clouds":%g})", //
        name, //
        flags.exists, flags.neutral, flags.read_only,
        d_temp, d_wind, d_humi, d_clouds);

    return n < dst_size ? n : 0;
  }
  bool from_json(const char *json);
  bool from_json(JsmnBase::Iterator &it);


public:
  char name[CONFIG_APP_FA_NAMES_MAX_LEN] = "";
  struct {
    bool exists = false;   ///< to mark as non existent flat value in array
    bool neutral = false;   ///< mark adapter as neutral (returning factor 1.0)
    bool read_only = false; ///< prevent adapter from beeing overwritten by user
  } flags;
  float d_temp, d_wind, d_humi, d_clouds;
};

