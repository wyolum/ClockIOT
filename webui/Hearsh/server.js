const express = require('express');
const bodyParser = require('body-parser');
const https = require('https');
const path = require('path');
const fs = require('fs');
const axios = require('axios');
const app = express();

const certOptions = {
  key: fs.readFileSync(path.resolve('certificate/server.key')),
  cert: fs.readFileSync(path.resolve('certificate/server.crt'))
}

app.get('/getIpData', (req, res) => {
	axios.get('https://www.wyolum.com/utc_offset/get_localips.py')
	.then(response => {
		let message = `Choose a ClockIOT`;
		let data = [];
		for(let ips of response.data.localips) {
			if(ips.dev_type === 'ClockIOT') {
				data.push(ips.localip);
			}
		}
		res.status(200);
  	res.json({
  		data,
  		message,
  	});
  });
});

const server = https.createServer(certOptions, app).listen(443);