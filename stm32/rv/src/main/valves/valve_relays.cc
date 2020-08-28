#include "user_config.h"
#include <valves/valve_relays.h>
#include "rv/report.h"

uint16_t valveRelay_getActiveValves() {
  uint16_t result = VALVE_RELAY_MASK & ~Mcp23017_getBits(&relay_16, VALVE_RELAY_MASK, true);
  return result;
}

void app_switch_valve(int valve_number, bool state) {
  bool old_bit = !Mcp23017_getBit(&relay_16, valve_number, true);
  Mcp23017_putBit(&relay_16, valve_number, (state ? RELAY_ON : RELAY_OFF) != 0);
  if (old_bit != state) {
    report_valve_status((1 << valve_number), (1 << valve_number));
  }
}

void app_switch_valves(uint16_t valve_bits, uint16_t valve_mask) {
  uint16_t old_bits = ~Mcp23017_getBits(&relay_16, valve_mask, true);
  Mcp23017_putBits(&relay_16, valve_mask, ~valve_bits);

  if ((valve_mask &= (old_bits ^ valve_bits))) { // report modified bits only
    report_valve_status(valve_bits, valve_mask);
  }
}
