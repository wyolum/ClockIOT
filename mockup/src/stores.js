import { readable, writable, derived } from 'svelte/store';

export const basicInfo = writable({})
export const timeInfo = writable({})
export const localTime = readable(new Date(), function start(set) {
		const interval = setInterval(() => {
			set(new Date());
		}, 1000);
		
		return function stop() {
			clearInterval(interval);
		}
	})
export const clockData = writable({})