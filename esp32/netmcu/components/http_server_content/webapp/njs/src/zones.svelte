<script>
"use strict";

import { ZoneCount, ZoneNames, ZoneDurations, ZoneRemainingSeconds } from './store/zones';
import Zone from './zone.svelte';
</script>

<table class="bg-green-400">
{#each { length:ZoneCount } as _, i}
<tr>
<th>{i}</th> 
<Zone zoneIdx={i}/>
</tr>
{/each} 

</table>