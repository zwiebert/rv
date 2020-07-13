<script>
"use strict";

import { ZoneCount, ZoneNames, ZoneDurations, ZoneRemainingSeconds } from './store/zones';


export let zoneIdx;
export let zoneNames;

$: name = zoneNames[zoneIdx] || ''; 
$: duration = ($ZoneDurations[zoneIdx] / 60.0).toFixed(2) || 0;
$: remaining = ($ZoneRemainingSeconds[zoneIdx] / 60.0).toFixed(2) || 0;

</script>

<td><input type="text" bind:value={zoneNames[zoneIdx]} /></td>
<td>{duration}</td>
<td>{remaining}</td>

