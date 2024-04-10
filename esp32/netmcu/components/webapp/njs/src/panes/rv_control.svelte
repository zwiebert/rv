<script>
  import * as httpFetch from "../app/fetch.js";
  import { _ } from "../services/i18n";
  import Zones from "../app/zones.svelte";
  import RvStatus from "../components/rv_status.svelte";
  import { Stm32Time } from "../store/zones.js";
</script>


<div class="main-area">

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
    <input type="text" value={$Stm32Time} />
  </div>

</div>
