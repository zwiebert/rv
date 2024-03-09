#include <weather/weather.hh>

#include <string>
#include <list>
#include <array>
#include <algorithm>
using namespace std;


constexpr unsigned MAX_DRY_DAYS = 30;

struct WetterDaten {
  bool rain_sensor;
  unsigned regensensor_trocken_dauer = MAX_DRY_DAYS; //Zeitdauer seitdem der Regensensor trocken wurdde
  float wetter_faktor;
  struct {
    float wind_faktor;
    float sonnen_faktor;
    float temp_faktor;
    float wolken_faktor;
  } wetter_faktoren;
};

using adaption_h_fun_type = int (*)(int haeufigkeit, const WetterDaten &wetterdaten);
using adaption_d_fun_type = int (*)(int normal_dauer,const WetterDaten &wetterdaten);

struct AdaptionH {
  string name;
  adaption_h_fun_type fun;
};

struct GruppenEigenschaften {
  int prio = 1; //ersetzt durch Listen-Reihenfolge // Priorität für Zeitnähe zur Morgendämmerung (Rasen=niedrig, BLumen=hoch)
  int haeufigkeit;  // wie oft pro Woche bewässert werden soll
  adaption_h_fun_type adaption_h;  //modifziert häufigkeit

};

struct EinzelKreisEigenschaften {
  int normal_dauer;  // Länge einer normalen Bewässerungseinheit (100%)
  int wasserdurchfluss_lph; // Liter pro Stunde. der stm32 nutzt das um Kreise zeitgleich zu öffnen)
  adaption_d_fun_type adaption_d;
};

struct EinzelKreis {
  string name;
  unsigned not_run_since = MAX_DRY_DAYS;  //Tage seit dieser Kreis nicht bewässert wurde (alles: auto und manuell!)
  struct EinzelKreisEigenschaften eke;
};

struct Gruppe {
  string name;  // Rasen/Blumen/Töpfe/Hecke/etc
  list<EinzelKreis*> kreise; // die einzelnen Kreise (MagnetVentile rv0..rv14), sortiert von zuerst (Nacht) bis zuletzt (Sonnenaufgang) (inverse prio)
  struct GruppenEigenschaften ge;  // Blumen/Rasen/Töpfe/Hecke mit ihren Eigenschaften


  bool ist_dran(const EinzelKreis &kreis, const WetterDaten &wd) {
    if (wd.rain_sensor)
      return false;
    unsigned how_often_d = ge.adaption_h(ge.haeufigkeit, wd);
    int last_watering = std::min(kreis.not_run_since, wd.regensensor_trocken_dauer);
    return (how_often_d <= last_watering);
  }
};

struct GartenPumpe {
  GartenPumpe();
  string name;
  int max_wasserdurchfluss_lph; // begrenzt anzahl der gleichzeitig offenen kreise
  int max_wasserentnahme_lph; // bei überschreiten einfühgen von Pausen
  EinzelKreis *ek_begin(), *ek_end(), *ek_get(unsigned idx);
  Gruppe *gr_begin(), *gr_end(), *gr_get(unsigned idx);

  //array<EinzelKreis> kreise; // alle Kreise
  //list<Gruppe> gruppen; // alle Gruppen
};

extern GartenPumpe brunnen_pumpe;
