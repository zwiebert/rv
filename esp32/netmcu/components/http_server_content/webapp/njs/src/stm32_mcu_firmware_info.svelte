<script>
  "use strict";
  import { Stm32McuFirmwareVersion } from "./store/mcu_firmware.js";
  import * as httpFetch from "./fetch.js";
  import { onMount, onDestroy } from "svelte";

  let on_destroy = [];
  onMount(() => {
    //httpFetch.http_fetchByMask(httpFetch.FETCH_VERSION);
  });
  onDestroy(() => {
    for (const fn of on_destroy) {
      fn();
    }
  });
</script>

<ul>
  <li>Version: {$Stm32McuFirmwareVersion}</li>
</ul>
