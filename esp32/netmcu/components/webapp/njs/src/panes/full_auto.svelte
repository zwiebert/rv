<script>
  import { _ } from "../services/i18n";
  import { Z, ZoneDurationMmss, ZoneRemainingMmss } from "../store/curr_zone";

  import * as httpFetch from "../app/fetch.js";
  import { onMount } from "svelte";
  import PollZoneData from "../app/poll_zone_data.svelte";
  import SelectZone from "../components/select_zone.svelte";
  import RvStatus from "../components/rv_status.svelte";
  import ZoneData from "../components/zone_data.svelte";
  import RvTimer from "../components/rv_timer.svelte";
  import { ZoneCount, ZoneNames } from "../store/zones";
  import Zone from "../app/rv_config_zone.svelte";


  onMount(() => {
    httpFetch.http_fetchByMask(httpFetch.FETCH_ZONE_NAMES);
  });

  onMount(() => {
    get_data();
  });

  $: auto_data = {};
  $: valves = [];
  $: adapters = [];
  $: auto_data_json = JSON.stringify(auto_data);

  $: sel_adapter_idx = 0;
  $: sel_adapter = adapters.length ? adapters[sel_adapter_idx] : null;
  $: sel_adapter_copy = {... sel_adapter };
  $: sel_valve_idx = 0;
  $: sel_valve = valves.length ? valves[sel_valve_idx] : null;
  $: sel_valve_copy = { ...sel_valve };
  $: sel_valve_tmp = { };


  function get_data() {
    const fetch_data = {
      method: "GET",
      cache: "no-cache",
      headers: { Accept: "application/json" },
      referrer: "no-referrer",
    };

    return fetch("/f/data/auto.json", fetch_data).then((response) => {
      if (response.ok) {
        response
          .json()
          .then((obj) => {
            auto_data = obj;
            valves = obj.valves;
            adapters = obj.adapters;
          })
          .catch((error) => {
            console.log("error: http_postRequest(): ", error);
          });
      }
    });
  }

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

  function f(v) {
    if (v == null) return "";
    if ("flags" in v) {
      return JSON.stringify(v.flags);
    }
    return "--";
  }

  function secs_to_dh(secs) {
    const hours = secs / (60*60);

    const d = (hours/24).toFixed(0);
    const hh = (hours % 24).toFixed(0);
    const mm = ((secs / 60)%60).toFixed(0);
    return `${d}d-${hh}h:${mm}m`;
  }
</script>

<div class="main-area">
  <div class="area">
    <select bind:value={sel_valve_idx}>
      {#each valves as v, i}
        {#if v !== null && v.flags.exists}
          <option value={i}>{v.name} </option>
        {/if}
      {/each}
    </select>

    {#if sel_valve !== null}
      <table class="border-solid">
        <tr>
          <th>Kvs-Name</th><td>{$ZoneNames[sel_valve_idx]}</td>
        </tr>
        <tr>
          <th>Priority</th><td>
            <input type="number" bind:value={sel_valve_copy.attr.priority} style="width:8ch;" />
          </td>
        </tr>
        <tr class="">
          <th>Adapter</th>
          <td>
            <select bind:value={sel_valve_copy.attr.adapter}>
              {#each adapters as v, i}
                {#if v !== null && v.flags.exists}
                  <option value={i}>{v.name} </option>
                {/if}
              {/each}
            </select>
          </td>
        </tr>
        <tr>
          <th>Interval</th><td>
            <input type="number" min="0" step="3600" bind:value={sel_valve_copy.attr.interval_s} style="width:8ch;" />
            {secs_to_dh(sel_valve_copy.attr.interval_s )}       
            </td
          >
        </tr>
        <tr>
          <th>Flow</th><td>
            <input type="number" min="0" bind:value={sel_valve_copy.attr.flow_lph} style="width:8ch;" />
          </td>
        </tr>
      </table>
    {/if}
  </div>

  <div class="area"></div>
  <select bind:value={sel_adapter_idx}>
    {#each adapters as v, i}
      {#if v !== null && v.flags.exists}
        <option value={i}>{v.name} </option>
      {/if}
    {/each}
  </select>

  {#if sel_adapter !== null}
    <table class="border-solid w-full">
      <tr>
        <th>Temp</th><td>{sel_adapter_copy.temp}</td>
      </tr>
      <tr>
        <th>Humi</th><td>{sel_adapter_copy.humi}</td>
      </tr>
      <tr>
        <th>Wind</th><td>{sel_adapter_copy.wind}</td>
      </tr>
      <tr>
      <tr>
        <th>Clouds</th><td>{sel_adapter_copy.clouds}</td>
      </tr>
    </table>
  {/if}
</div>
