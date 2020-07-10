'use strict';
import { writable, derived } from 'svelte/store';
import { OptionStore, PersistentValStore} from './custom_stores.js';


export const McuConfig = OptionStore();
export const McuConfigKeys = derived(McuConfig, mcuConfig => Object.keys(mcuConfig) || []); // XXX: ordering of keys?
