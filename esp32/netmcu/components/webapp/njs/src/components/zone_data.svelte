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
    let key = "lph"+$Z;
    let val = lph;
    let msg = {kvs:{}};
    msg[key] = val;
    console.log(msg);
    httpFetch.sendRv(msg);
  }

  $: lph = $ZoneLPH;

</script>

<table>
  <tr><th>LPH</th><td><input type="number" step="1" bind:value={lph}></td></tr>
</table>

<button on:click={postZoneData}>{$_('app.save')}</button>

