<script>
  import { _ } from "../services/i18n";
  import {
    PressControlStatus,
    WaterPumpStatus,
    RainSensorStatus,
  } from "../store/zones";
  import * as httpFetch from "../fetch.js";
  import { onMount } from "svelte";

  onMount(() => {
    httpFetch.http_fetchByMask(httpFetch.FETCH_RV_STATUS);
  });
</script>

<label>
  {$_('app.pressControl')}
  <input type="checkbox" checked={$PressControlStatus} />
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
