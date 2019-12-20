<script>
import InfoTable from './InfoTable.svelte';
var clockInfo = {
	hostname: 'localhost',
	port: '81',
}
</script>

<InfoTable title="Basic info" data={clockInfo}/>