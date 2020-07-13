<script>
  "use strict";

  import * as httpFetch from "./fetch.js";
  import NavTabs from "./nav_tabs.svelte";
  import Zones from "./zones.svelte";
  import Pump from "./pump.svelte";
  import { TabIdx } from "./store/app_state.js";
  import Layout from "./layout.svelte";
  import McuConfig from "./mcu_config.svelte";
  import McuFirmwareUpd from "./mcu_firmware_upd.svelte";
  import McuFirmwareInfo from "./mcu_firmware_info.svelte";

  export let isProduction = false;

  $: tabIdxMain = $TabIdx["main"] || 0;
  $: tabIdxSettings = $TabIdx["settings"] || 0;
  $: visibleMcuConfig = false;
  $: visiblePairedControllers = false;
  $: visibleMovementDurations = false;

  let navTabs = ["RV", "Config", "Firmware"];

  let fwbtns = [
    { name: "latest master firmware", ota_name: "github-master" },
    { name: "latest beta firmware", ota_name: "github-beta" },
  ];

  let stm32_fwbtns = [
    { name: "latest master firmware", ota_name: "stm32-github-master" },
    { name: "latest beta firmware", ota_name: "stm32-github-beta" },
  ];

  // eslint-disable-next-line no-unused-labels
  testing: if (!isProduction) {
    fwbtns.push({
      name: "firmware from given URL",
      ota_name: "netotaFromURL",
      input: "input",
      input_value: "http://192.168.1.70:8000/netmcu.bin",
    });
    stm32_fwbtns.push({
      name: "firmware from given URL",
      ota_name: "netotaFromURL",
      input: "input",
      input_value: "http://192.168.1.70:8000/rv.bin",
    });
  }
</script>

<style type="text/scss">
  @import "./styles/app.scss";

  .area {
    @apply mt-4 p-2 bg-area;
  }
</style>

<Layout />

<div id="navTabs" class="flex flex-col items-center px-1 border-none">
  <NavTabs nav_tabs={navTabs} name="main" />

  {#if tabIdxMain === 0}
    <div class="area">
      <Zones />
      <Pump />
      <button on:click={() => httpFetch.http_fetchByMask(httpFetch.FETCH_ZONE_DATA|httpFetch.FETCH_ZONE_NAMES)}>Reload</button>
    </div>
  {:else if tabIdxMain === 1}
    <div class="area">
      <McuConfig />
    </div>
  {:else if tabIdxMain === 2}
    <div class="area">
      <h4>NetMCU ESP32 Firmware Update</h4>
      <McuFirmwareUpd {fwbtns} chip="esp32" />
      <h4>Firmware Info</h4>
      <McuFirmwareInfo />
   </div>
<br>
   <div class="area">
      <h4>RvMCU STM32 Firmware Update</h4>
      <McuFirmwareUpd fwbtns={stm32_fwbtns} chip="stm32" />
    </div>
  {/if}
</div>
