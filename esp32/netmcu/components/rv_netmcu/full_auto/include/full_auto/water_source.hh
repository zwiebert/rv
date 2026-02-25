#include <vector>
#include <list>

namespace app::fa {
struct WaterSource  {
  string m_name;
  int m_water_flow_limit_lph; // begrenzt anzahl der gleichzeitig offenen kreise
  int m_water_volume_limit_lph; // bei überschreiten einfühgen von Pausen
};
} // namespace



