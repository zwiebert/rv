<script>
  import { _ } from "../services/i18n";
  import * as misc from "../misc.js";
  import McuFirmwareUpd from "../mcu_firmware_upd.svelte";
  import McuFirmwareInfo from "../mcu_firmware_info.svelte";

  export let isProduction = false;

  let fwbtns = [
    { name: "latest master", ota_name: "github-master" },
    { name: "latest beta", ota_name: "github-beta" },
  ];

  // eslint-disable-next-line no-unused-labels
  testing: if (!isProduction) {
    fwbtns.push({
      name: "from URL",
      ota_name: "netotaFromURL",
      input: "input",
      input_value: "http://192.168.1.76:8000/netmcu.bin",
    });
  }
</script>

<div class="area">
  <McuFirmwareUpd {fwbtns} chip="" updSecs="14" />
  <McuFirmwareInfo />
  <button type="button" on:click={() => misc.req_mcuRestart()}>
    {$_('app.restartMcu')}
  </button>
</div>
