<script>
    "use strict";
    import { _ } from './services/i18n';
    import {
      ZoneCount,
      ZoneNames,
    } from "./store/zones";
    import Zone from "./rv_config_zone.svelte";
    import * as httpFetch from "./fetch.js";
    import { onMount} from "svelte";
  
    onMount(() => {
      httpFetch.http_fetchByMask(httpFetch.FETCH_ZONE_NAMES | httpFetch.FETCH_ZONE_DATA);
    });
  
  
  
  function postZoneNames() {
    let netmcu = {to:"netmcu"};
    let kvs = {zn:'?'};
    netmcu.kvs = kvs;
  
    for (let i=0; i < ZoneCount; ++i) {
      let sfx = i.toString();
      let key = 'zn'+sfx;
      let new_name =  zoneNames[i];
      let old_name = $ZoneNames[i];
      if (old_name !== new_name) {
        kvs[key] = new_name;
      }
    }
  
    console.log(JSON.stringify(netmcu));
    let url = '/cmd.json';
    console.log("url: "+url);
    httpFetch.http_postRequest(url, netmcu);
  }
  
  
    $: zoneNames = [...$ZoneNames];
  
  </script>
  
  <table class="bg-transparent">
    <tr>
      <th>#</th>
      <th>{$_('app.zoneName')}</th>
      <th>{$_('app.hide')}</th>
    </tr>
    {#each { length: ZoneCount } as _, i}
      <tr>
        <th>{i}</th>
        <Zone zoneIdx={i} {zoneNames} />
      </tr>
    {/each}
  
  </table>
  
  <button on:click={postZoneNames}>{$_('app.save')}</button>
  <br>