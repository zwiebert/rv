'use strict';
import { derived } from 'svelte/store';
import { OptionStore } from './custom_stores.js';


export const McuConfig = OptionStore();
export const McuConfigKeys = derived(McuConfig, mcuConfig => Object.keys(mcuConfig) || []); // XXX: ordering of keys?
