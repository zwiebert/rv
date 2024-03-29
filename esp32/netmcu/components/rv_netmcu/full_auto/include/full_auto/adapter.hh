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
struct WeatherAdapter: public Weather_Adapter_Neutral {
  using self_type = WeatherAdapter;

  WeatherAdapter() :
      d_temp(0.06), d_wind(0.001), d_humi(0.01), d_clouds(0.01) {
  }
  WeatherAdapter(const WeatherAdapter &) = default;
  WeatherAdapter &operator=(const WeatherAdapter &) = default;

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
        flags.exists, flags.neutral, flags.read_only, d_temp, d_wind, d_humi, d_clouds);

    return n < dst_size ? n : 0;
  }

  template<typename T, typename std::enable_if<!std::is_class<T>{},bool>::type = true>
  bool from_json(T json) {
    auto jsmn = Jsmn<32, T>(json);

    if (!jsmn)
      return false;

    auto it = jsmn.begin();
    return from_json(it);
  }

  template<typename jsmn_iterator = Jsmn_String::Iterator, typename std::enable_if<std::is_class<jsmn_iterator>{},bool>::type = true>
  bool from_json(jsmn_iterator &it) {
    assert(it->type == JSMN_OBJECT);

    using token_handler_fun_type = bool (*)(self_type &self, jsmn_iterator &it, int &err);
    static const token_handler_fun_type tok_processRootChilds_funs[] = { //

        [](self_type &self, jsmn_iterator &it, int &err) -> bool {
          if (it.keyIsEqual("flags", JSMN_OBJECT)) {
            auto count = it[1].size;
            for (it += 2; count > 0 && it; --count) {
              if (it.getValue(self.flags.exists, "exists") //
              || it.getValue(self.flags.neutral, "neutral") //
                  || it.getValue(self.flags.read_only, "read_only") //
                      ) {
                it += 2;
              } else {
                ++err;
                it.skip_key_and_value();
              }
            }
            return true;
          }
          return false;

        },

        [](self_type &self, jsmn_iterator &it, int &err) -> bool {
          if (it.getValue(self.name, "name") || it.getValue(self.d_temp, "temp") || it.getValue(self.d_wind, "wind") || it.getValue(self.d_humi, "humi")
              || it.getValue(self.d_clouds, "clouds")) {
            it += 2;
            return true;
          }
          return false;
        },

        [](self_type &self, jsmn_iterator &it, int &err) -> bool { // Throw away unwanted objects
          return it.skip_key_and_value();
        } };

    int err = 0;
    auto count = it->size;
    for (++it; count > 0 && it; --count) {
      for (auto fun : tok_processRootChilds_funs) {
        if (fun(*this, it, err))
          break;
      }
    }
    return !err;

  }

public:
  char name[CONFIG_APP_FA_NAMES_MAX_LEN] = "";
  struct {
    bool exists = false;   ///< to mark as non existent flat value in array
    bool neutral = false;   ///< mark adapter as neutral (returning factor 1.0)
    bool read_only = false; ///< prevent adapter from beeing overwritten by user
  } flags;
  float d_temp, d_wind, d_humi, d_clouds;
};

