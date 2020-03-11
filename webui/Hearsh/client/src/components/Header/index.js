import React from 'react';
import styles from './styles';
import PropTypes from 'prop-types';

const Header = (props) => {
	return (
		<header style={styles.header}>
			<h1 style={styles.h1}>
				{props.message}
			</h1>
		</header>
	)
}

Header.propType = {
	message: PropTypes.string.isRequired,
}

export default Header;
