<script>
  "use strict";
  import { Stm32McuFirmwareVersion } from "./store/mcu_firmware.js";
  import * as httpFetch from "./fetch.js";
  import { onMount } from "svelte";

  onMount(() => {
    httpFetch.http_fetchByMask(httpFetch.FETCH_VERSION);
  });

</script>

<ul>
  <li>Version: {$Stm32McuFirmwareVersion}</li>
</ul>
