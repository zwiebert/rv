<script>
  import { _ } from "../services/i18n";
  import { Z, ZoneDurationMmss, ZoneRemainingMmss } from "../store/curr_zone";

  import * as httpFetch from "../fetch.js";
  import { onMount } from "svelte";
  import PollZoneData from "../poll_zone_data.svelte";
  import SelectZone from "../components/select_zone.svelte";
  import RvStatus from "../components/rv_status.svelte";
  import ZoneData from "../components/zone_data.svelte";
  import RvTimer from "../components/rv_timer.svelte";

  onMount(() => {
    httpFetch.http_fetchByMask(httpFetch.FETCH_ZONE_NAMES | httpFetch.FETCH_ZONE_DURATIONS | httpFetch.FETCH_ZONE_REMAINING_DURATIONS);
  });

  function cmdDuration(args) {
    let cmd = {};
    cmd.timer = args;
    httpFetch.sendRv(cmd);
  }

  function onClickRun() {
    cmdDuration({ vn: $Z, d1: 60, ir: true, ip: true });
  }
  function onClickStop() {
    cmdDuration({ vn: $Z });
  }
</script>

<PollZoneData />

<div class="main-area">

  <div class="area">
    <SelectZone />
  </div>

  <div class="area">
    <button class="text-xl p-8 m-2" on:click={onClickRun}>Start</button>
    <button class="text-xl p-8 m-2" on:click={onClickStop}>Stop</button>
  </div>

  <div class="area">
    <p>{$_('app.duration')}: {$ZoneDurationMmss} {$_('app.remaining')}: {$ZoneRemainingMmss}</p>
  </div>

  <div class="area">
    <RvStatus />
  </div>

  <div class="area">
    <ZoneData />
  </div>

  <div class="area">
    <h4>Timer</h4>
    <RvTimer />
  </div>

</div>
