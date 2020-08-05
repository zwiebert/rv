import { derived } from 'svelte/store';
import { PersistentIntStore } from './custom_stores.js';
import { ZoneNames, ZoneDurations, ZoneRemainingSeconds } from './zones.js';
import { formatSecondsToMMSS } from "../misc.js";

export const Z = PersistentIntStore("group");
export const ZoneName = derived([Z, ZoneNames], ([z, names]) => names[z] || '');

export const ZoneDuration = derived([Z, ZoneDurations], ([z, durations]) => durations[z] || 0);
export const ZoneDurationMmss = derived(ZoneDuration, duration => formatSecondsToMMSS(duration));
export const ZoneRemaining = derived([Z, ZoneRemainingSeconds], ([z, remaining]) => remaining[z] || 0);
export const ZoneRemainingMmss = derived(ZoneRemaining, remaining => formatSecondsToMMSS(remaining));