<script>
  import { ZoneLPHs } from "../store/zones.js";
  import * as httpFetch from "../fetch.js";
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
