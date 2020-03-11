import React from 'react';
import PropTypes from 'prop-types';
import styles from './styles';

const Display = (props) => {
	return(
		<button 
			id='displayBtn' 
			value={props.display} 
			onClick={() => props.sendMessage('display_idx', props.id)}
			style={styles.btns}
		> 
			{props.display} 
		</button>
	);
}

const DisplayList = (props) => {
	return(
		<div id='myDisplays' style = {styles.myDisplays}>
			{props.displays.map((display, index) => 
				<Display 
					key = {index} 
					id = {index} 
					display = {display} 
					sendMessage = {props.sendMessage} 
				/>
				)}
		</div>
	);
}

export default DisplayList;