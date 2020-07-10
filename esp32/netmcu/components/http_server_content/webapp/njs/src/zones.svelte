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
      <Zone zoneIdx={i} />
    </tr>
  {/each}

</table>
