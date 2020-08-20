<script>
  import { _ } from "../services/i18n";
  import {
    ZoneLPHs, ZoneDataMsg
  } from "../store/zones";
  import {
    Z, ZoneLPH,
  } from "../store/curr_zone";
  import * as McuComPb from "../pbuf/mcu_com_pb.js";
  import * as Base64 from "js-base64";
  import * as httpFetch from "../fetch.js";
  import { onMount } from "svelte";

  onMount(() => {
    httpFetch.http_fetchByMask(httpFetch.FETCH_ZONE_DATA);
  });

  function postZoneData() {
    $ZoneLPHs[$Z] = lph; 
    let msg = $ZoneDataMsg;
    if (!msg)
       return;
    msg.setLphList($ZoneLPHs);
    let buf = msg.serializeBinary();
    let buf64 = Base64.fromUint8Array(buf);
    let cmd = { "zd": buf64};
    console.log(cmd);
    httpFetch.sendPbuf(cmd);
  }

  $: lph = $ZoneLPH;

</script>

<table>
  <tr><th>LPH</th><td><input type="number" step="1" bind:value={lph}></td></tr>
</table>

<button on:click={postZoneData}>{$_('app.save')}</button>

