<script>
  import * as httpFetch from "../fetch.js";
  import { _ } from "../services/i18n";
  import Zones from "../zones.svelte";
  import Pump from "../pump.svelte";
  import { Stm32Time } from "../store/zones.js";
  import PollZoneData from "../poll_zone_data.svelte";
</script>

<PollZoneData />

<div class="area">
  <Zones />
  <Pump />
  <button
    on:click={() => httpFetch.http_fetchByMask(httpFetch.FETCH_ZONE_DATA | httpFetch.FETCH_ZONE_NAMES)}>
    {$_('app.reload')}
  </button>
  <br />
  <br />
  <input type="text" value={$Stm32Time} />
</div>
