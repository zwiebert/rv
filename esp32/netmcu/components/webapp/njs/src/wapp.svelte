<script>
  "use strict";
  import { _ } from "./services/i18n";
  import NavTabs from "./nav_tabs.svelte";

  import PaneRvControl from "./panes/rv_control.svelte";
  import PaneRvSettings from "./panes/rv_settings.svelte";
  import PaneMcuSettings from "./panes/mcu_settings.svelte";
  import PaneFirmwareEsp32 from "./panes/firmware_esp32.svelte";
  import PaneFirmwareStm32 from "./panes/firmware_stm32.svelte";
  import { TabIdx } from "./store/app_state.js";

  export let isProduction = false;

  $: tabIdxMain = $TabIdx["main"] || 0;
  $: tabIdxCfg = $TabIdx["cfg"] || 0;
  $: tabIdxFw = $TabIdx["fw"] || 0;
  $: tabIdxSettings = $TabIdx["settings"] || 0;
</script>

<style type="text/scss">
  @import "./styles/app.scss";
</style>

<div id="navTabs" class="flex flex-col items-center px-1 border-none">
  <div class="navtab-main">
    <NavTabs
      nav_tabs={[$_('app.nav_main_rv'), $_('app.nav_main_config'), $_('app.nav_main_firmware')]}
      name="main" />
  </div>
  {#if !tabIdxMain}
    <PaneRvControl />
  {:else if tabIdxMain === 1}
    <div class="navtab-sub">
      <NavTabs
        nav_tabs={[$_('app.nav_cfg_netMcu'), $_('app.nav_cfg_rv')]}
        name="cfg" />
    </div>
    {#if !tabIdxCfg}
      <PaneMcuSettings />
    {:else}
      <PaneRvSettings />
    {/if}
  {:else if tabIdxMain === 2}
    <div class="navtab-sub">
      <NavTabs
        nav_tabs={[$_('app.nav_fw_netMcu'), $_('app.nav_fw_rv')]}
        name="fw" />
    </div>
    {#if !tabIdxFw}
      <PaneFirmwareEsp32 {isProduction} />
    {:else if tabIdxFw === 1}
      <PaneFirmwareStm32 {isProduction} />
    {/if}
  {/if}
</div>
