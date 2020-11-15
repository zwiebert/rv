<script>
  import { _ } from "services/i18n";
  import {
    ZoneLPHs, ZoneDataMsg
  } from "stores/zones";
  import {
    Z, ZoneLPH,
  } from "stores/curr_zone";
  import * as McuComPb from "main/pbuf/mcu_com_pb.js";
  import * as Base64 from "js-base64";
  import * as httpFetch from "app/fetch.js";
  import { onMount } from "svelte";

  onMount(() => {
    httpFetch.http_fetchByMask(httpFetch.FETCH_ZONE_LPHS);
  });

  function postZoneData() {
    let key = "lph"+$Z;
    let val = document.getElementById("lph").value;
    let msg = {config:{}};
    msg.config[key] = val;
    console.log(msg);
    httpFetch.sendRv(msg);
    
    msg = {};
    msg["lph"+$Z] = "?";
    setTimeout(() => httpFetch.sendKvs(msg), 500);
  }

  $: lph = $ZoneLPHs[$Z];
  
</script>

<table>
  <tr><th>LPH</th><td><input type="number" id="lph" step="1" value={$ZoneLPHs[$Z]}></td></tr>
</table>

<button on:click={postZoneData}>{$_('app.save')}</button>

