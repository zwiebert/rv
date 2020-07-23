<script>
  import { _ } from "../services/i18n";
  import { Z, ZoneName } from "../store/curr_zone";
  import {
    ZoneCount,
  } from "../store/zones";
  import * as httpFetch from "../fetch.js";
  import { onMount } from "svelte";

  onMount(() => {
    httpFetch.http_fetchByMask(httpFetch.FETCH_ZONE_NAMES);
  });
</script>

<label>{$_('app.zone')}: {$ZoneName}</label>
<br />
<select bind:value={$Z}>
  {#each { length: $ZoneCount } as _, zn}
    <option>{zn}</option>
  {/each}

</select>
