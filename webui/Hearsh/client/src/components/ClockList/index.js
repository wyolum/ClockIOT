import React from 'react';
import PropTypes from 'prop-types';
import styles from './styles';

const ListClockIps = (props) => {
	return (
		<option value={props.value}>{props.value}</option>
	)
}

const ClockList = (props) => {
	return(
		<div style={styles.ipList}>
			<select id='clock-list' style={styles.select} onChange={props.handleChangeOfIp.bind(this)}>
				<option value=''>Select an IP</option>
				{props.ips.map(ip => <ListClockIps key={ip} value={ip} />)}
			</select>
		</div>
	)
}

ClockList.propType = {
	ips: PropTypes.array.isRequired,
	handleChangeOfIp: PropTypes.func,
}

export default ClockList;