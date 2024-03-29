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
    get_data();
  });

  $: auto_data = { valves:[], adapters:[] };
  $: zones = [...auto_data.valves];
  $: adapters = [...auto_data.adapters];
  $: auto_data_json = JSON.stringify(auto_data);
  $: sel_adapter_idx = 0;
  $: sel_zone_idx = 0;

  $: {
    zones;
    console.log("new zones");
  }


  function resp_data(obj) {
            auto_data = {... obj};
  }
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
            resp_data(obj);
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
    const hours = secs / (60 * 60);

    const d = (hours / 24).toFixed(0);
    const hh = (hours % 24).toFixed(0);
    const mm = ((secs / 60) % 60).toFixed(0);
    return `${d}d-${hh}h:${mm}m`;
  }

  function save_zone() {
    const key = "zone." + sel_zone_idx;
    let obj = { json: { auto: {} } };
    obj.json.auto[key] = zones[sel_zone_idx];
    httpFetch.http_postRequest("/cmd.json", obj);
  }

  function enable_zone(idxs) {
    let obj = { json: { auto: {} } };
    for(let idx of idxs) {
      let zobj = {flags:{exists:1}};
      zobj.name = "New zone " + idx;
    const key = "zone." + idx;
    obj.json.auto[key] = zobj; 
    }
    httpFetch.http_postRequest("/cmd.json", obj);
  }

</script>

<div class="main-area">
  <div class="area">

      <button
        type="button"
          on:click={()=> enable_zone([1,2,3,4])}>Add Zone 4</button>
          <br>

    <select bind:value={sel_zone_idx}>
      {#each zones as v, i}
        {#if v !== null && v.flags.exists}
          <option value={i}>{v.name} </option>
        {/if}
      {/each}
    </select>

    {#if sel_zone_idx < zones.length && zones[sel_zone_idx]}
      <table class="border-solid">
        <tr>
          <th>Kvs-Name</th><td>{$ZoneNames[sel_zone_idx]}</td>
        </tr>
          <th>Name</th><td>
            <input type="text" bind:value={zones[sel_zone_idx].name} style="width:20ch;" />
          </td>
        <tr>
          <th>Priority</th><td>
            <input type="number" bind:value={zones[sel_zone_idx].attr.priority} style="width:8ch;" />
          </td>
        </tr>
        <tr class="">
          <th>Adapter</th>
          <td> <select bind:value={zones[sel_zone_idx].attr.adapter}>
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
            <input type="number" min="0" step="3600" bind:value={zones[sel_zone_idx].attr.interval_s} style="width:8ch;" />
            {secs_to_dh(zones[sel_zone_idx].attr.interval_s)}
          </td>
        </tr>
        <tr>
          <th>Flow</th><td>
            <input type="number" min="0" bind:value={zones[sel_zone_idx].attr.flow_lph} style="width:8ch;" />
          </td>
        </tr>
      </table>
      <button
        type="button"
        on:click={get_data}>Reload</button
      >
      <button
        type="button"
        on:click={save_zone}>Save</button
      >
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

  {#if sel_adapter_idx < adapters.length && adapters[sel_adapter_idx]}
    <table class="border-solid w-full">
      <tr>
        <th>Temp</th><td>{adapters[sel_adapter_idx].temp}</td>
        <td><input type="range"></td>
      </tr>
      <tr>
        <th>Humi</th><td>{adapters[sel_adapter_idx].humi}</td>
        <td><input type="range"></td>
      </tr>
      <tr>
        <th>Wind</th><td>{adapters[sel_adapter_idx].wind}</td>
        <td><input type="range"></td>
      </tr>
      <tr> </tr><tr>
        <th>Clouds</th><td>{adapters[sel_adapter_idx].clouds}</td>
        <td><input type="range"></td>
      </tr>
    </table>
      <button
        type="button"
        on:click={get_data}>Reload</button
      >
    <button
      type="button"
      on:click={() => {
        const key = "adapter." + sel_adapter_idx;
        let obj = { json: { auto: {} } };
        obj.json.auto[key] = adapters[sel_adapter_idx];
        httpFetch.http_postRequest("/cmd.json", obj);
      }}>Save</button
    >
  {/if}
</div>
