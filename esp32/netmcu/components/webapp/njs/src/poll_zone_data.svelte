<script>
  import {
    PressControlStatus,
    WaterPumpStatus,
    RainSensorStatus,
  } from "./store/zones";
  import * as httpFetch from "./fetch.js";
  import { onMount, onDestroy } from "svelte";
  import { ReloadProgress } from './store/app_state.js';

  export let pollInterval = 5000;


  let on_destroy = [];
  onMount(() => {
    httpFetch.http_fetchByMask(httpFetch.FETCH_CONFIG);
  });
  onDestroy(() => {
    stop_polling();
    for (const fn of on_destroy) {
      fn();
    }
  });

  $: {
    if ($PressControlStatus) start_polling();
    else stop_polling();
  }

  let interval = 0;

  function start_polling() {
    interval = setInterval(poll_data, pollInterval);
  }

  function stop_polling() {
    clearInterval(interval);
    interval = 0;
  }

  function poll_data() {
    httpFetch.http_fetchByMask(httpFetch.FETCH_ZONE_DATA);
  }
</script>
