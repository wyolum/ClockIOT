<script>
	export let title = '';
	export let data;
	if (typeof data === 'undefined') {
		throw new Error('No data provided to InfoTable');
	}
	let cells = [];
	for (let [label, value] of Object.entries(data)) {
		cells.push({
			label,
			value
		})
	}
</script>

<style>
	table, td {
		border: 1px solid #333;
		border-collapse: collapse;
	}
	td {
		padding: 5px;
	}
</style>

<table class="infobox">
	{#if title !== ''}
	<thead class="infobox-head">
		<tr>
			<th colspan=2>{title}</th>
		</tr>
	</thead>
	{/if}
	<tbody class="infobox-body">
		{#if typeof data === 'string'}
		<tr class="infocell">{data}</tr>
		{:else}
			{#each Object.entries(data) as [label, value]}
			<tr class="infocell">
				<td class="infocell-label">{label}</td>
				{#if typeof value === 'string' || typeof value === 'number' || typeof value === 'boolean'}
				<td class="infocell-value">{value}</td>
				{:else}
				<td class="infocell-value" style={value.style}>{value.text}</td>
				{/if}
			</tr>
			{/each}
		{/if}
	</tbody>
</table>
