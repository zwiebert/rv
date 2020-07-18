<script>
  "use strict";
  import {
    PressControlStatus,
    WaterPumpStatus,
    RainSensorStatus,
  } from "./store/zones";
</script>

<label>
  PC
  <input
    type="checkbox"
    id="id-pressControlStatus"
    checked={$PressControlStatus} />
</label>
<br />
<label>
  Pump
  <input type="checkbox" id="id-waterPumpStatus" checked={$WaterPumpStatus} />
</label>
<br />
<label>
  Rain-Sensor
  <input type="checkbox" id="id-rainSensorStatus" checked={$RainSensorStatus} />
</label>
<br />
