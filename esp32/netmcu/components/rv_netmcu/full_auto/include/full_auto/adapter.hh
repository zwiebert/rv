#pragma once

#include <weather/weather_data.hh>
#include <weather/weather_irrigation.hh>
#include "jsmn/jsmn_iterate.hh"
#include "jsoneat/from_json_jsmn.hh"
#include <string>

struct location_data {

};
/***
 * \brief Adapt Irrigation according to weather data
 */
struct WeatherAdapter: public Weather_Adapter_Neutral {
  using self_type = WeatherAdapter;

  WeatherAdapter() :
      d_temp(0.06), d_wind(0.001), d_humi(0.01), d_clouds(0.01) {
  }
  WeatherAdapter(const WeatherAdapter&) = default;
  WeatherAdapter& operator=(const WeatherAdapter&) = default;

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
  /**
   * \brief            serialize object into JSON formatted null-terminated string.
   * \param dst        output buffer
   * \param dst_size   output buffer size
   * \return           return values are the as standard snprintf(3). The bytes written or if larger than dst_size, the bytes which would have written.
   *                   the terminating null byte is not counted, but its always there even if a data bytes has to be truncated for it.
   *                   so make sure to add one byte to the return value before using it as a parameter for dst_size in a retry call.
   */
  int to_json(char *dst, size_t dst_size) const {
    return snprintf(dst, dst_size, //
        R"({"name":"%s","flags":{"exists":%d,"neutral":%d,"read_only":%d},"temp":%g,"wind":%g,"humi":%g,"clouds":%g})", //
        name, //
        flags.exists, flags.neutral, flags.read_only, d_temp, d_wind, d_humi, d_clouds);
  }

  template<typename jsmn_iterator>
  bool from_json(jsmn_iterator &it) {
    assert(it->type == JSMN_OBJECT);

    // if JSON value is null, re-initialize object
    if ((it + 1).value_equals_null()) {
      *this = WeatherAdapter();
      return true;
    }

    return jsoneat::from_json::jsmn::deserialize_object(it, JSONEAT_KvPairs(name, flags), jsoneat::KvPair("temp", d_temp),
        jsoneat::KvPair("wind", d_wind),
        jsoneat::KvPair("humi", d_humi), jsoneat::KvPair("clouds", d_clouds));
  }

public:
  char name[CONFIG_APP_FA_NAMES_MAX_LEN] = "";
  struct {
    bool exists = false;   ///< to mark as non existent flat value in array
    bool neutral = false;   ///< mark adapter as neutral (returning factor 1.0)
    bool read_only = false; ///< prevent adapter from beeing overwritten by user

    template<typename jsmn_iterator>
    bool from_json(jsmn_iterator &it) {
    return jsoneat::from_json::jsmn::deserialize_object(it, JSONEAT_KvPairs(exists, neutral, read_only));
    }

  } flags;
  float d_temp, d_wind, d_humi, d_clouds;
};

