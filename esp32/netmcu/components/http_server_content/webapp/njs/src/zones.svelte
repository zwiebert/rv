<script>
  "use strict";

  import {
    ZoneCount,
    ZoneNames,
    ZoneDurations,
    ZoneRemainingSeconds,
  } from "./store/zones";
  import Zone from "./zone.svelte";
  import * as httpFetch from "./fetch.js";
  import { onMount, onDestroy } from "svelte";

  let on_destroy = [];
  onMount(() => {
    httpFetch.http_fetchByMask(httpFetch.FETCH_ZONE_NAMES | httpFetch.FETCH_ZONE_DATA);
  });
  onDestroy(() => {
    for (const fn of on_destroy) {
      fn();
    }
  });


function postZoneNames() {
  let netmcu = {to:"netmcu"};
  let kvs = {};
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

<table class="bg-green-400">
  <tr>
    <th>#</th>
    <th>Name</th>
    <th>Duration</th>
    <th>Remaining</th>
  </tr>
  {#each { length: ZoneCount } as _, i}
    <tr>
      <th>{i}</th>
      <Zone zoneIdx={i} {zoneNames} />
    </tr>
  {/each}

</table>

<button on:click={postZoneNames}>Save</button>
<br>