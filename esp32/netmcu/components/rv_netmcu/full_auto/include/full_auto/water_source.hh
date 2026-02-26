#include <vector>
#include <list>

namespace app::fa {


  /**
     * \brief  Represent a water source (e.g. well pump)
   */
struct WaterSource  {
  string m_name; ///< Give water source a name
  int m_water_flow_limit_lph; ///< limit how many zones can run in parallel
  int m_water_volume_limit_lph; ///< add pauses between zones when volume limit reached
};
} // namespace
