<script>
	import { onMount, onDestroy } from 'svelte';
	import ClockInfo from './ClockInfo.svelte';
	import InfoTable from './InfoTable.svelte';
	import { localTime, clockData } from './stores';

	var conn;
	var data = {};

	function handleData(msg) {
		try {
			let data = JSON.parse(msg.data);
			clockData.set(data);
			window.debugData = data;
			console.log('Clock data updated.');
		} catch(error) {
			if (error instanceof SyntaxError) {
				console.warn('JSON parse failure. Data follows: ')
				console.log(msg.data);
			}
		}
	}

	onMount(() => {
		conn = new WebSocket('ws://localhost:81');
		conn.addEventListener('message', handleData)
		conn.addEventListener('open', () => {
			conn.send('debug');
		})
	})

	onDestroy(() => {
		conn.close();
	})

	function refreshData() {
		conn.send('debugOnce');
	}

	function normalizeOffset(minuteOffset) {
		// new Date().getTimezoneOffset() returns the timezone offset in minutes and the offset is multiplied by -1 for some reason
		let hourOffset = Math.floor(minuteOffset / 60);
		minuteOffset = (String)(Math.abs(minuteOffset) % 60).padStart(2, 0);
		let negative = hourOffset < 0;
		hourOffset = (String)(Math.abs(hourOffset)).padStart(2, 0);
		if (!negative) hourOffset = '-' + hourOffset;
		return hourOffset + minuteOffset;
	}
	let localOffset = normalizeOffset(new Date().getTimezoneOffset());
	let displayData = $clockData.display;
	if (typeof displayData === 'undefined') {
		displayData = 'fetching...'
	} else {
		Object.assign(displayData, {
			color: {
				style: `background-color: #${$clockData.color};`,
				text: `#${clockData.color}`
			},
			faceplate: $clockData.faceplate
		})
	}
	
</script>

<h1>ClockIOT Mockup Dev View</h1>
<p>
	This is served from the mockup server and should display the info about the fake clock that it represents.
</p>
<div class="container">
	<div class="vcontainer">
		<ClockInfo/>
		<InfoTable title="Time data" data={{
			"Clock Timezone": $clockData.timezone || 'fetching...',
			"IP Timezone": $clockData.ipOffset || 'fetching...',
			"Local Timezone": localOffset,
			}} />
		<InfoTable title="Display info" data={$clockData.displayData || 'fetching...'} />
	</div>
	<InfoTable title="Server info" data={$clockData.ipData || 'loading..'} />
</div>
<button id="refreshData" on:click={refreshData}>Refresh data</button>