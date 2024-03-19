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

  onMount(() => {
    get_data();
  });

  $: auto_data = {};
  $: valves = [];
  $: adapters = [];
  $: auto_data_json = JSON.stringify(auto_data);

  $: sel_adapter_idx = 0;
  $: sel_adapter = adapters.length ? adapters[sel_adapter_idx] : null
  $: sel_valve_idx = 0;
  $: sel_valve = valves.length ? valves[sel_valve_idx] : null;

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
</script>

<div class="main-area">
  <div class="area">
    <table>
      <tr><th>Active</th><th>Name</th><th>Interval</th></tr>
      {#each valves as v, i}
        {#if v !== null && v.flags.exists}
          <tr>
            <td><input type="checkbox" checked={v.flags.active} /></td>
            <td>{i + ": " + v.name} </td>
            <td>{v.attr.interval_s / (60 * 60)}</td>
          </tr>
        {/if}
      {/each}
    </table>
  </div>

  <div class="area">
    {#each adapters as v, i}
      {#if v !== null && v.flags.exists}
        {i + ": " + v.name}
        <input type="checkbox" checked={v.flags.active} />

        <br />
      {/if}
    {/each}

  </div>

  <div class="area">
    <select bind:value={sel_adapter_idx}>
      {#each adapters as v, i}
        {#if v !== null && v.flags.exists}
          <option value={i}>{v.name} </option>
        {/if}
      {/each}
    </select>
  </div>

  <div class="area">
    <select bind:value={sel_valve_idx}>
      {#each valves as v, i}
        {#if v !== null && v.flags.exists}
          <option value={i}>{v.name} </option>
        {/if}
      {/each}
    </select>

    {#if sel_valve !== null}
    <ul>
      <li>{sel_valve.name}</li>
      <li>{JSON.stringify(sel_valve.attr)}</li>
      <li>{sel_valve.attr.flow_lph}</li>
    </ul>
    
    {/if}
  </div>


</div>
