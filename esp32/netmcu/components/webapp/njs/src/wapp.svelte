<script>
  "use strict";
  import { _ } from './services/i18n';
  import * as misc from "./misc.js";
  import * as httpFetch from "./fetch.js";
  import NavTabs from "./nav_tabs.svelte";
  import Zones from "./zones.svelte";
  import Pump from "./pump.svelte";
  import { TabIdx } from "./store/app_state.js";
  import McuConfig from "./mcu_config.svelte";
  import RvConfig from "./rv_config.svelte";
  import McuFirmwareUpd from "./mcu_firmware_upd.svelte";
  import McuFirmwareInfo from "./mcu_firmware_info.svelte";
  import Stm32McuFirmwareInfo from "./stm32_mcu_firmware_info.svelte";

  import { Stm32Time } from "./store/zones.js";

  export let isProduction = false;

  $: tabIdxMain = $TabIdx["main"] || 0;
  $: tabIdxCfg = $TabIdx["cfg"] || 0;
  $: tabIdxFw = $TabIdx["fw"] || 0;
  $: tabIdxSettings = $TabIdx["settings"] || 0;
  $: visibleMcuConfig = false;
  $: visiblePairedControllers = false;
  $: visibleMovementDurations = false;

  let fwbtns = [
    { name: "latest master", ota_name: "github-master" },
    { name: "latest beta", ota_name: "github-beta" },
  ];

  let stm32_fwbtns = [
    { name: "latest master", ota_name: "stm32-github-master" },
    { name: "latest beta", ota_name: "stm32-github-beta" },
  ];

  // eslint-disable-next-line no-unused-labels
  testing: if (!isProduction) {
    fwbtns.push({
      name: "from URL",
      ota_name: "netotaFromURL",
      input: "input",
      input_value: "http://192.168.1.76:8000/netmcu.bin",
    });
    stm32_fwbtns.push({
      name: "from URL",
      ota_name: "stm32_netotaFromURL",
      input: "input",
      input_value: "http://192.168.1.76:8000/rv.bin",
    });
  }
</script>

<style type="text/scss">
  @import "./styles/app.scss";
</style>

<div id="navTabs" class="flex flex-col items-center px-1 border-none">
  <div class="navtab-main">
    <NavTabs nav_tabs={[$_('app.nav_main_rv'), $_('app.nav_main_config'), $_('app.nav_main_firmware')]} name="main" />
  </div>
  {#if tabIdxMain === 0}
    <div class="area">
      <Zones />
      <Pump />
      <button
        on:click={() => httpFetch.http_fetchByMask(httpFetch.FETCH_ZONE_DATA | httpFetch.FETCH_ZONE_NAMES)}>
        {$_('app.reload')}
      </button>
      <br />
      <br />
      <input type="text" value={$Stm32Time} />
    </div>
  {:else if tabIdxMain === 1}
  <div class="navtab-sub">
    <NavTabs nav_tabs={[$_('app.nav_cfg_netMcu'), $_('app.nav_cfg_rv')]} name="cfg" />
  </div>
  {#if tabIdxCfg === 0}
    <div class="area">
      <McuConfig />
    </div>
  {:else}
  <div class="area">
    <RvConfig />
  </div>
  {/if}
  {:else if tabIdxMain === 2}
    <div class="navtab-sub">
      <NavTabs nav_tabs={[$_('app.nav_fw_netMcu'), $_('app.nav_fw_rv')]} name="fw" />
    </div>
    {#if tabIdxFw === 0}
      <div class="area">
        <McuFirmwareUpd {fwbtns} chip="" updSecs="14" />
        <McuFirmwareInfo />
        <button type="button" on:click={() => misc.req_mcuRestart()}>
          {$_('app.restartMcu')}
        </button>
      </div>
    {:else if tabIdxFw === 1}
      <div class="area">
        <McuFirmwareUpd fwbtns={stm32_fwbtns} chip="stm32" updSecs="75" />
        <Stm32McuFirmwareInfo />
        <button type="button" on:click={() => misc.req_stm32McuRestart()}>
          {$_('app.restartMcu')}
        </button>
      </div>
    {/if}
  {/if}
</div>
