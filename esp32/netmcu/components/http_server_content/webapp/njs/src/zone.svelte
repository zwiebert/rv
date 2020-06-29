<script>
"use strict";

import { ZoneCount, ZoneNames, ZoneDurations, ZoneRemainingSeconds } from './store/zones';


export let zoneIdx;

$: name = $ZoneNames[zoneIdx] || ''; 
$: duration = $ZoneDurations[zoneIdx] || 0;
$: remaining = $ZoneRemainingSeconds[zoneIdx] || 0;

</script>

<td>{name}</td>
<td>{duration}</td>
<td>{remaining}</td>

