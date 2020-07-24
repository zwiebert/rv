<script>
  import * as httpFetch from "../fetch.js";
  import { _ } from "../services/i18n";
  import Zones from "../zones.svelte";
  import RvStatus from "../components/rv_status.svelte";
  import { Stm32Time } from "../store/zones.js";
  import PollZoneData from "../poll_zone_data.svelte";
</script>

<PollZoneData />

<div class="area">

  <div class="area">
    <Zones />

  </div>

  <div class="area">
    <RvStatus />

  </div>

  <div class="area">
    <button
      on:click={() => httpFetch.http_fetchByMask(httpFetch.FETCH_ZONE_DURATIONS | httpFetch.FETCH_ZONE_REMAINING_DURATIONS | httpFetch.FETCH_ZONE_NAMES | httpFetch.FETCH_RV_STATUS)}>
      {$_('app.reload')}
    </button>
    <br />
    <br />
    <input type="text" value={$Stm32Time} />
  </div>

</div>
