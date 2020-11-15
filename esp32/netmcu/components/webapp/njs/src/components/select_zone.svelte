<script>
  import { _ } from "services/i18n";
  import { Z, ZoneName } from "stores/curr_zone";
  import { ZoneCount, ZoneNames, ZoneDisabled } from "stores/zones";
  import * as httpFetch from "app/fetch.js";
  import { onMount } from "svelte";

  onMount(() => {
    httpFetch.http_fetchByMask(httpFetch.FETCH_ZONE_NAMES);
  });
</script>

<select bind:value={$Z}>
  {#each { length: $ZoneCount } as _, zn}
    {#if ($ZoneDisabled & (1 << zn)) === 0}
      <option value={zn}>{zn} {$ZoneNames[zn] || ''}</option>
    {/if}
  {/each}
</select>
