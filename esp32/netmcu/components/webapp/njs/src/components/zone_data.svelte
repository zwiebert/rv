<script>
  import { _ } from "../services/i18n";
  import {
    ZoneLPHs,
  } from "../store/zones";
  import {
    ZoneLPH,
  } from "../store/curr_zone";
  import * as httpFetch from "../fetch.js";
  import { onMount } from "svelte";

  onMount(() => {
    httpFetch.http_fetchByMask(httpFetch.FETCH_ZONE_DATA);
  });
</script>

<table>
  <tr><th>LPH</th><td><input type="number" step="1" value={$ZoneLPH}></td></tr>
</table>

