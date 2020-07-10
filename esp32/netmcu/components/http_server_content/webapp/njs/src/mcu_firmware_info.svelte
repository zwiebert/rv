<script>
  "use strict";
  import {
    McuBootCount,
    McuFirmwareBuildDate,
    McuChipId,
    McuFirmwareVersion,
  } from "./store/mcu_firmware.js";
  import * as httpFetch from "./fetch.js";
  import { onMount, onDestroy } from "svelte";

  let on_destroy = [];
  onMount(() => {
    httpFetch.http_fetchByMask(
      httpFetch.FETCH_VERSION
    );
  });
  onDestroy(() => {
    for (const fn of on_destroy) {
      fn();
    }
  });
</script>



<ul>
   <li>MCU type: {$McuChipId}</li>
    <li>Version: {$McuFirmwareVersion}</li>
    <li>Build Date: {$McuFirmwareBuildDate}</li>
    <li>Boot Count: {$McuBootCount}</li>
</ul>

