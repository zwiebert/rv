#include <full_auto/setup.hh>
#include <weather/weather_provider_owm.hh>

FullAuto *full_auto;

void fa_setup(fa_cfg *cfg) {
  full_auto = new FullAuto(new Weather_Provider_Owm);
}
