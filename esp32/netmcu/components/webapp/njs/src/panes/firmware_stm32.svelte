<script>
  import { _ } from "../services/i18n";
  import * as misc from "../misc.js";
  import McuFirmwareUpd from "../mcu_firmware_upd.svelte";
  import Stm32McuFirmwareInfo from "../stm32_mcu_firmware_info.svelte";

  let stm32_fwbtns = [
    { name: "latest master", ota_name: "stm32-github-master" },
    { name: "latest beta", ota_name: "stm32-github-beta" },
  ];

  if (misc.NODE_ENV_DEV) {
    stm32_fwbtns.push({
      name: "from URL",
      ota_name: "stm32_netotaFromURL",
      input: "input",
      input_value: "http://192.168.1.76:8000/rv.bin",
    });
  }
</script>

<div class="area">
  <McuFirmwareUpd fwbtns={stm32_fwbtns} chip="stm32" updSecs="75" />
  <Stm32McuFirmwareInfo />
  <button type="button" on:click={() => misc.req_stm32McuRestart()}>
    {$_('app.restartMcu')}
  </button>
</div>
