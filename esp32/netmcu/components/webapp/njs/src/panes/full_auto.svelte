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

  $: auto_data = { valves: [], adapters: [] };
  $: zones = [...auto_data.valves];
  $: adapters = [...auto_data.adapters];
  $: auto_data_json = JSON.stringify(auto_data);
  $: sel_adapter_idx = 0;
  $: sel_zone_idx = -1;
  let zones_exists = [];
  function set_zone_exists(idx, val) {
    zones_exists[idx] = val;
  }
  $: {
    zones;
    console.log("new zones");
    for (let i = 0; sel_zone_idx < 0 && i < zones.length; ++i) {
      if (zones[i] !== null) {
        sel_zone_idx = i;
      }
    }
    for (let i = 0; i < zones.length; ++i) {
      set_zone_exists(i, zones[i] !== null && zones[i].flags.exists);
    }
  }
  $: {
    if (zones[sel_zone_idx] && "attr" in zones[sel_zone_idx]) sel_adapter_idx = zones[sel_zone_idx].attr.adapter;
  }

  function get_modified_existences() {
    let result = [];
    for (let i = 0; i < zones.length; ++i) {
      if ((zones_exists[i] && (zones[i] === null || !zones[i].flags.exists)) || (!zones_exists[i] && zones[i] !== null && zones[i].flags.exists)) {
        result.push(i);
      }
    }
    console.log("modified flags.exists", result);
    return result;
  }
  function resp_data(obj) {
    auto_data = { ...obj };
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
    for (let idx of idxs) {
      let zobj = { flags: { exists: 1 } };
      zobj.name = "New zone " + idx;
      const key = "zone." + idx;
      obj.json.auto[key] = zobj;
    }
    httpFetch.http_postRequest("/cmd.json", obj);
  }

  function adapter_add(idxs) {
    let obj = { json: { auto: {} } };
    for (let idx of idxs) {
      let aobj = { flags: { exists: 1 } };
      aobj.name = "New adapter " + idx;
      const key = "adapter." + idx;
      obj.json.auto[key] = aobj;
    }
    httpFetch.http_postRequest("/cmd.json", obj);
  }
</script>

<div class="main-area">

    <table>
      <tr>
        {#each zones as v, i}
          <th>{i}</th>
        {/each}
      </tr>
      {#if zones_exists.length > 0}
        <tr>
          {#each zones as v, i}
            <td><input type="checkbox" bind:checked={zones_exists[i]} /></td>
          {/each}
        </tr>
      {/if}
    </table>
    <button
      type="button"
      on:click={() => {
        const mel = get_modified_existences();
        let obj = { json: { auto: {} } };
        for (let i of mel){
          const key = "zone."+i; 
          obj.json.auto[key] = {flags:{exists:zones_exists[i]}} ;
        }
        httpFetch.http_postRequest("/cmd.json", obj);
      }}>Apply</button
    >

  <div class="area">
    <select bind:value={sel_zone_idx}>
      {#each zones as v, i}
        {#if v !== null && v.flags.exists}
          <option value={i}>{i}:{v.name} </option>
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
          <td>
            <select bind:value={zones[sel_zone_idx].attr.adapter}>
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
      <button type="button" on:click={get_data}>Reload</button>
      <button type="button" on:click={save_zone}>Apply</button>
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

  <button
    type="button"
    on:click={() => {
      for (let i = 0; i < adapters.length; ++i) {
        if (adapters[i] === null) {
          adapter_add([i]);
          break;
        }
      }
    }}>+</button
  >

  {#if sel_adapter_idx < adapters.length && adapters[sel_adapter_idx]}
    <table class="border-solid w-full">
      <tr>
        <th>Name</th><td>
          <input type="text" disabled={adapters[sel_adapter_idx].flags.read_only} bind:value={adapters[sel_adapter_idx].name} style="width:20ch;" />
        </td>
      </tr>
      <tr>
        <th>Temp</th><td>{adapters[sel_adapter_idx].temp}</td>
        <td
          ><input
            type="range"
            disabled={adapters[sel_adapter_idx].flags.read_only}
            bind:value={adapters[sel_adapter_idx].temp}
            min="0.006"
            max="0.6"
            step="0.01"
          /></td
        >
      </tr>
      <tr>
        <th>Humi</th><td>{adapters[sel_adapter_idx].humi}</td>
        <td
          ><input
            type="range"
            disabled={adapters[sel_adapter_idx].flags.read_only}
            bind:value={adapters[sel_adapter_idx].humi}
            min="0.001"
            max="0.1"
            step="0.001"
          /></td
        >
      </tr>
      <tr>
        <th>Wind</th><td>{adapters[sel_adapter_idx].wind}</td>
        <td
          ><input
            type="range"
            disabled={adapters[sel_adapter_idx].flags.read_only}
            bind:value={adapters[sel_adapter_idx].wind}
            min="0.0001"
            max="0.01"
            step="0.001"
          /></td
        >
      </tr>
      <tr> </tr><tr>
        <th>Clouds</th><td>{adapters[sel_adapter_idx].clouds}</td>
        <td
          ><input
            type="range"
            disabled={adapters[sel_adapter_idx].flags.read_only}
            bind:value={adapters[sel_adapter_idx].clouds}
            min="0.001"
            max="0.1"
            step="0.001"
          /></td
        >
      </tr>
    </table>
    <button type="button" on:click={get_data}>Reload</button>
    <button
      type="button"
      on:click={() => {
        get_modified_existences();
        const key = "adapter." + sel_adapter_idx;
        let obj = { json: { auto: {} } };
        obj.json.auto[key] = adapters[sel_adapter_idx];
        httpFetch.http_postRequest("/cmd.json", obj);
      }}>Apply</button
    >
  {/if}
  <hr />
  <button
    type="button"
    on:click={() => {
      let obj = { json: { auto: { command: { save: "" } } } };
      httpFetch.http_postRequest("/cmd.json", obj);
    }}>Save</button
  >

  <button
    type="button"
    on:click={() => {
      let obj = { json: { auto: { command: { restore: "" } } } };
      httpFetch.http_postRequest("/cmd.json", obj);
    }}>Restore</button
  >
</div>
