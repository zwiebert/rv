<script>
  "use strict";
  import { _ } from './services/i18n';
  import {
    PressControlStatus,
    WaterPumpStatus,
    RainSensorStatus,
  } from "./store/zones";
</script>

<label>
  {$_('app.pressControl')}
  <input
    type="checkbox"
    id="id-pressControlStatus"
    checked={$PressControlStatus} />
</label>
<br />
<label>
  {$_('app.pump')}
  <input type="checkbox" id="id-waterPumpStatus" checked={$WaterPumpStatus} />
</label>
<br />
<label>
  {$_('app.rainSensor')}
  <input type="checkbox" id="id-rainSensorStatus" checked={$RainSensorStatus} />
</label>
<br />
