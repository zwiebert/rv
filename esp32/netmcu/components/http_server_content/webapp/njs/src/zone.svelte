<script>
  "use strict";

  import { ZoneDurations, ZoneRemainingSeconds } from "./store/zones";
  import * as httpFetch from "./fetch.js";

  export let zoneIdx;
  export let zoneNames;

  $: duration = ($ZoneDurations[zoneIdx] / 60.0).toFixed(2) || 0;
  $: remaining = ($ZoneRemainingSeconds[zoneIdx] / 60.0).toFixed(2) || 0;
  $: checked = duration > 0.01;

  function onClickRun() {
    let cmd = {};
    let key = "dur" + zoneIdx;
    cmd[key] = document.getElementById("zrun" + zoneIdx).checked
      ? document.getElementById(key).valueAsNumber
      : 0;
    httpFetch.sendCmd(cmd);
  }
</script>

<td>
  <input class="w-32" type="text" bind:value={zoneNames[zoneIdx]} />
</td>
<td>
  <input class="w-16" type="number" value={duration} id="dur{zoneIdx}" />
</td>
<td>
  <input type="checkbox" {checked} on:click={onClickRun} id="zrun{zoneIdx}" />
</td>
<td>{remaining}</td>
