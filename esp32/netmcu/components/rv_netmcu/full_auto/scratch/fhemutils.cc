#include "weather/weather_provider.hh"

#include <assert.h>
#include <stdint.h>

#include <time.h>

extern Weather_Provider &weather;

float rvPoints[24]; // TODO
unsigned rain_sensor_last_yday = 0; //TODO

bool get_rainSensorCurrentState() {
  return false; // TODO
}

unsigned rvd[16] = { //
    20, // rv0 lawn_west
        9,  // rv1 flow_hortens
        7, // rv2 flow_south
        20, //rv3 lawn_east
        10, //rv4 kaktus
        9, //rv5 flow_east
        20, //rv6 lawn_north
        12, //rv7 hedge_east
        5, //rv8 flow_northeast
        20, //rv9 flow_west
        11, //rv10 flow_north
        1, // rv11 pot_north
    };

typedef unsigned (*getInterval_fun_T)(float wpt);

struct rv_group {
  const char name[32];
  union {
    int16_t bd;
    struct {
      uint16_t rv0 :1;
      uint16_t rv1 :1;
      uint16_t rv2 :1;
      uint16_t rv3 :1;
      uint16_t rv4 :1;
      uint16_t rv5 :1;
      uint16_t rv6 :1;
      uint16_t rv7 :1;
      uint16_t rv8 :1;
      uint16_t rv9 :1;
      uint16_t rv10 :1;
      uint16_t rv11 :1;
      uint16_t rv12 :1;
      uint16_t rv13 :1;
      uint16_t rv14 :1;
      uint16_t rv15 :1;
    } vb;
  } val;
};

const int NMB_GROUPS = 4;

rv_group rv_groups[NMB_GROUPS] = {  //
    { .name = "lawns", .val = { .vb = { .rv0 = 1, .rv3 = 1, .rv6 = 1 } } }, //
        { .name = "flowers", .val = { .vb = { .rv1 = 1, .rv5 = 1, .rv8 = 1, .rv9 = 1, .rv10 = 1 } } }, //
        { .name = "hedges", .val = { .vb = { .rv7 = 1 } } }, //
        { .name = "pots", .val = { .vb = { .rv11 = 1 } } }, //
    };

getInterval_fun_T getInterval_funs[NMB_GROUPS] = { [](float wpt) -> unsigned {
  return wpt > 17 ? 3 : (wpt > 15) ? 4 : (wpt > 10) ? 5 : 7;
},
[](float wpt) -> unsigned {
  return wpt > 14 ? 1 : (wpt > 12.5) ? 2 : (wpt > 11) ? 3 : 5;
},
[](float wpt) -> unsigned {
  return wpt > 15 ? 2 : (wpt > 13) ? 3 : (wpt > 11) ? 4 : 7;
},
[](float wpt) -> unsigned {
  return wpt > 10 ? 2 : 10;
},

};

float rv_getWeather();

bool rv_fetchWeather(weather_data &w) {
  return weather.fetch_weather_data(w);
}

unsigned rv_getInterval(unsigned group) {
  assert(group < NMB_GROUPS);
  float wpt = rv_getWeather();
  return (*getInterval_funs[group])(wpt);
}

void set_rvpt(float rvpt) {
  auto now = time(nullptr);
  auto tm = localtime(&now);
  rvPoints[tm->tm_hour] = rvpt;
}
float get_rvpt(unsigned hour) {
  assert(hour < 24);
  return rvPoints[hour];
}
/**
 * \brief save the last day of the year the rain sensor was active
 */
void rv_sample_rainsensor() {
  // TODO
}

bool rv_sampleWeather() {
  weather_data wd;
  if (!rv_fetchWeather(wd))
    return false;

  float rvpt = 1.0 * wd.get_temp_celsius() * (wd.get_wind_speed_kph() / 30.0 + 1)
      / (wd.get_cloud_coverage_percent() / 100.0 + wd.get_relative_humidity_percent() / 100.0 + 1);
  set_rvpt(rvpt);
  rv_sample_rainsensor();
  return true;
}

float rv_getWeather() {
  int n = 0;
  float sum = 0.0;
  for (int i = 9; i <= 21; ++i) {
    float pt = get_rvpt(i);
    if (pt > 0.001) {
      ++n;
      sum += pt;
    }
  }
  if (!n)
    return 0.0;
  return sum / n;
}

#if 0

sub rv_setPercent($$) {
	my ($name, $pct) = @_;
	print "setPct: name=$name, pct=$pct\n";
	return unless defined $rvd->{$name};
	my ($dev, $minutes) = @{$rvd->{$name}};
	
	$minutes *= $pct / 100.0;
	main::fhem("set $dev dauer $minutes"); 
}

sub rv_setPercentGroup($$) {
	my ($group_name, $pct) = @_;
	for my $name (@{$rv_groups->{$group_name}}) {
	rv_setPercent($name, $pct);
	}
}





sub rv_get_info() {
	#my @groups = keys(%Srv::rv_groups);
    my $tnow = localtime();
	my @groups = ("Lawn", "Flow", "Hedge");
	for my $g (@groups) {
	   my $iv = rv_getInterval($g);
	   my $lyd = main::getKeyValue("rv${g}LastYday") - $tnow->yday;
	   main::fhem ("setreading rv_info interval_$g $iv");
	   main::fhem ("setreading rv_info last_day_$g $lyd");
	}
}


sub rv_do_group($$@) {
	my ($name, $force, @group) = @_;
	my $tnow = localtime();
	my $rain = main::ReadingsVal("rvRain", "rain", 0);
	my $key = "rv${name}LastYday";
	my ($error, $lastYday) = main::getKeyValue($key);
	$lastYday = 0 unless defined $lastYday;
	$lastYday = 0 if $tnow->yday < $lastYday;
	my $interval = rv_getInterval($name);
	my ($error2, $rainLastYday) = main::getKeyValue("rvRainSensorLastYday");

    #$lastYday = $rainLastYday if ($rainLastYday && $lastYday < $rainLastYday);

	my $wpt = rv_getWeather();
	print("rv${name}: wpt=$wpt, rain=$rain, interval=$interval, lastYday=$lastYday\n");
	
	if (!$rain) {
		return unless $force || (($lastYday + $interval) <= $tnow->yday);
		
		my $mult = $force ? $force : 1.0;
		return unless 0.0 < $mult && $mult <= 2.0; 
	
	    for my $rv (@group) {
			my ($dev, $minutes) = @$rv;
			$minutes *= $mult;
			main::fhem("set $dev dauer $minutes"); 
		}
	}
	
	if ($rain || !$force || $force >= 0.95) {
	  main::setKeyValue($key, $tnow->yday);
	}
}

sub rv_do_lawns {
    my ($force) = @_;
	rv_do_group("Lawn", $force, @RV_LAWNS);
}

sub rv_do_flowers {
    my ($force) = @_;
	rv_do_group("Flow", $force, @RV_FLOWERS);
}

sub rv_do_hedges {
    my ($force) = @_;
	rv_do_group("Hedge", $force, @RV_HEDGES);
}

sub rv_do_pots {
    my ($force) = @_;
	rv_do_group("Pots", $force, @RV_POTS);
}

sub rv_switch_on($$) {
	my ($rv, $minutes) = @_; 

    $minutes = 10 unless defined $minutes;

   if (defined $rv) { 
   		main::fhem("set rv$rv dauer $minutes"); 
   } 
}

sub rv_switch_off($) { 
    my ($rv) = @_;
	rv_switch_on($rv, 0);
}


sub rv_switch {
   my ($event, $rv) = @_;
   rv_switch_on($rv, 10) if $event eq 'on';
   rv_switch_off($rv) if $event eq 'off';
}


sub rv_init() {
  main::fhem('defmod at_rvWeather at +*01:00 {rv::rv_sampleWeather()}');
}
#endif
