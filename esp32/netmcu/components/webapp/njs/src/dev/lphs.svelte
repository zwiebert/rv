<script>
  import { ZoneLPHs } from "stores/zones.js";
  import * as httpFetch from "app/fetch.js";
  import { onMount } from "svelte";

onMount(() => {
  httpFetch.http_fetchByMask(httpFetch.FETCH_ZONE_DATA);
});
</script>

<table>
  {#each $ZoneLPHs as lph}
    <tr>
      <td>{lph}</td>
    </tr>
  {/each}
</table>
