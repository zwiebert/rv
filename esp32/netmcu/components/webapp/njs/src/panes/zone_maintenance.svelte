<script>
  import { _ } from "../services/i18n";
  import {
    Z,
    ZoneName,
    ZoneDurationMmss,
    ZoneRemainingMmss,
  } from "../store/curr_zone";
  import {
    ZoneNames,
    ZoneCount,
    ZoneDurations,
    ZoneRemainingSeconds,
    PressControlStatus,
    WaterPumpStatus,
    RainSensorStatus,
  } from "../store/zones";
  import * as httpFetch from "../fetch.js";
  import { onMount } from "svelte";
  import PollZoneData from "../poll_zone_data.svelte";

  onMount(() => {
    httpFetch.http_fetchByMask(
      httpFetch.FETCH_ZONE_NAMES | httpFetch.FETCH_ZONE_DATA
    );
  });

  $: name = $ZoneName || "";

  function shn_fromHtml_toMcu(val) {
    let rd = { to: "netmcu", kvs: { zn: "?" } };
    rd.kvs["zn" + $Z.toString()] = val;

    let url = "/cmd.json";
    httpFetch.http_postRequest(url, rd);
  }

  function hChange_Name() {
    shn_fromHtml_toMcu(name);
  }

  //         sscanf(val, "%f,%d,%f,%d,%f,%d,%f,%f", &on, &ignoreRainSensor, &off, &repeats, &period, &dInterval, &dhBegin, &dhEnd);
  function cmdDuration(
    zone,
    onTime = 0.0,
    ignoreRainSensor = 0,
    offTime = 0.0,
    repeats = 0,
    period = 0.0,
    dInterval = 0,
    dhBegin = 0,
    dhEnd = 0
  ) {
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
    cmdDuration($Z, 1, 1);
  }
  function onClickStop() {
    cmdDuration($Z, 0);
  }
</script>

<PollZoneData />

<div class="area">

  <label>Zone: {$ZoneName}</label>
  <br />
  <select bind:value={$Z}>
    {#each { length: $ZoneCount } as _, zn}
      <option>{zn}</option>
    {/each}

  </select>
</div>

<div class="area">
  <div class="text-center">
    <label>{$_('app.msg_enterNewName')}</label>
    <br />
    <input type="text" name="name" bind:value={name} on:change={hChange_Name} />
  </div>
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
  <label>
    {$_('app.pressControl')}
    <input type="checkbox" checked={$PressControlStatus} />
  </label>
  <br />
  <label>
    {$_('app.pump')}
    <input type="checkbox" id="id-waterPumpStatus" checked={$WaterPumpStatus} />
  </label>
  <br />
  <label>
    {$_('app.rainSensor')}
    <input
      type="checkbox"
      id="id-rainSensorStatus"
      checked={$RainSensorStatus} />
  </label>
  <br />
</div>
