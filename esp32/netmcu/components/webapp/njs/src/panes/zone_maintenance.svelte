<script>
  import { _ } from "../services/i18n";
  import { Z, ZoneDurationMmss, ZoneRemainingMmss } from "../store/curr_zone";

  import * as httpFetch from "../fetch.js";
  import { onMount } from "svelte";
  import PollZoneData from "../poll_zone_data.svelte";
  import SelectZone from "../components/select_zone.svelte";
  import RvStatus from "../components/rv_status.svelte";

  onMount(() => {
    httpFetch.http_fetchByMask(
      httpFetch.FETCH_ZONE_NAMES |
        httpFetch.FETCH_ZONE_DURATIONS |
        httpFetch.FETCH_ZONE_REMAINING_DURATIONS
    );
  });

  //         sscanf(val, "%f,%d,%f,%d,%f,%d,%f,%f", &on, &ignoreRainSensor, &off, &repeats, &period, &dInterval, &dhBegin, &dhEnd);
  function cmdDuration(zone, args) {
    let onTime = args.onTime || 0.0;
    let ignoreRainSensor = args.ignore || 0;
    let offTime = args.offTime || 0.0;
    let repeats = args.repeats || 0;
    let period = args.period || 0.0;
    let dInterval = args.dInterval || 0;
    let dhBegin = args.dhBegin || 0;
    let dhEnd = args.dhEnd || 0;

    let cmdString =
      onTime +
      "," +
      ignoreRainSensor +
      "," +
      offTime +
      "," +
      repeats +
      "," +
      period +
      "," +
      dInterval +
      "," +
      dhBegin +
      "," +
      dhEnd;

    let cmd = {};
    let key = "dur" + zone;
    cmd[key] = cmdString;
    httpFetch.sendCmd(cmd);
  }

  function onClickRun() {
    cmdDuration($Z, { onTime: 1, ignore: 3 });
  }
  function onClickStop() {
    cmdDuration($Z, {});
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
    <p>
      {$_('app.duration')}: {$ZoneDurationMmss} {$_('app.remaining')}: {$ZoneRemainingMmss}
    </p>
  </div>

  <div class="area">
    <RvStatus />
  </div>

</div>
