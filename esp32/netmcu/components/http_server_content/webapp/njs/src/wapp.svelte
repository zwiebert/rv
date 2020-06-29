<script>
  import NavTabs from "./nav_tabs.svelte";
  import Zones from './zones.svelte';
  import { TabIdx } from "./store/app_state.js";
  import Layout from "./layout.svelte";

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

  // eslint-disable-next-line no-unused-labels
  testing: if (!isProduction) {
    navTabs.push("Tests");
    fwbtns.push({
      name: "firmware from given URL",
      ota_name: "netotaFromURL",
      input: "input",
      input_value: "http://192.168.1.70:3000/tronferno-mcu.bin",
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
    </div>
  {:else if tabIdxMain === 1}
    <div class="area">
   
    </div>
  {:else if tabIdxMain === 2}

    <div class="area">
  
    </div>

  {/if}
</div>
