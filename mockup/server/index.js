const WebSocket = require('ws');
const Enmap = require('enmap');
const rp = require('request-promise-native');
const settings = require('./settings');

// TODO: add event listener to watch for changes on enmap and send to subscribed clients for debug/dev interface.

const wss = new WebSocket.Server({
  port: 81
});

const clockMock = new Enmap('clock');
setDefaults();

// sets defaults without overwriting existing values
function setDefaults() {
  clockMock.ensure('timezone_offset', 'IP');
  clockMock.ensure('actual_offset', new Date().getTimezoneOffset());
  clockMock.ensure('ip', '127.0.0.1');
  clockMock.ensure('display', 0);
  clockMock.ensure('display_flipped', false);
  clockMock.ensure('faceplate', 1);
  clockMock.ensure('brightness', 128);
  clockMock.ensure('color', 'ffffff');
}

// clears db and sets defaults
function resetClock() {
  clockMock.clear();
  setDefaults();
}

function parseCommand(command, payload) {
  switch (command) {
    case 'timezone_offset':
      if (typeof payload === undefined || payload.length === 0) {
        return clockMock.get('timezone_offset');
      } else {
        let offset = parseInt(payload);
        payload = payload === 'IP' ? payload : parseInt(payload);
        if (payload === 'IP' || (offset >= -9999 && offset <= 9999)) {
          clockMock.set('timezone_offset', payload);
        }
      }
      break;
    case 'add_to_timezone':
      clockMock.math('timezone_offset', '+', parseInt(payload));
      break;
    case 'get_faceplates':
      return JSON.stringify({
        faceplates: settings.faceplates,
        faceplate_idx: clockMock.get('faceplate')
      });
    case 'faceplate_idx':
      payload = parseInt(payload);
      if (payload >= 0 && payload < settings.faceplates.length) {
        clockMock.set('faceplate', payload);
      }
      break;
    case 'get_displays':
      return JSON.stringify({
        displays: settings.displays,
        display_idx: clockMock.get('display')
      });
    case 'display_idx':
      payload = parseInt(payload);
      if (payload >= 0 && payload < settings.faceplates.length) {
        clockMock.set('display', payload);
      }
      break;
    case 'next_display':
      let display = clockMock.get('display');
      display += 1;
      if (display >= settings.displays.length) display = 0;
      clockMock.set('display', display);
      break;
    case 'flip_display':
      clockMock.set('display_flipped', !clockMock.get('display_flipped'));
      break;
    case 'request_ip':
      return settings.ip;
    case 'reset':
      resetClock();
      break;
    case 'set_rgb':
      clockMock.set('color', payload)
      break;
    default:
      throw new TypeError('Unknown command');
  }
}

function enumData() {
  let data = {};
  data.timezone = clockMock.get('timezone_offset');
  data.displayData = {
    id: clockMock.get('display'),
    flipped: clockMock.get('display_flipped'),
    faceplate: clockMock.get('faceplate'),
    color: {
      text: '#' + clockMock.get('color'),
      style: `background-color: #${clockMock.get('color')}`
    }
  };
  data.ipOffset = clockMock.get('ip_offset');
  data.faceplate = clockMock.get('faceplate');
  data.ipData = clockMock.get('ip_data');
  Object.assign(data, settings);
  return data;
}

// register the script as a real clock on the wyolum servers
function registerClock() {
  rp('https://wyolum.com/utc_offset/utc_offset.py?localip=localhost&dev_type=ClockIOT&macaddress=de:ad:be:ef:de:ad:be:ef')
    .then(response => {
      response = response.replace(',\n}', '\n}');
      let data = JSON.parse(response);
      clockMock.set('ip_offset', data.utc_offset);
      clockMock.set('ip_data', data);
    })
}
registerClock();

wss.on('connection', function connection(ws) {
  ws.send('Connected');
  ws.on('message', function incoming(message) {
    // matches clockiot/$1//$2
    // with everything after the first part optional
    // $1 is the command
    // $2 is the payload for some commands
    const pattern = /clockiot\/(\w+)(?:\/\/(\w+))?/

    console.log('recieved: %s', message);

    if (message === 'debug') {
      // send debug info every 2 seconds
      let interval = setInterval(() => {
        ws.send(JSON.stringify(enumData()));
      }, 2000);
      ws.send(JSON.stringify(enumData()));
      // when the connection closes, stop sending info
      ws.addEventListener('close', () => {
        clearInterval(interval);
        console.log('Debug client closed');
      });
      console.log('Debug client registered');
      return;
    }

    if (message === 'debugOnce') {
      ws.send(JSON.stringify(enumData()));
      return;
    }

    // match is null if invalid command
    // 2 long array if command found, with 0 being whole string and 1 being the command
    // 3 long array if payload found as well with 2 being the payload
    let match = message.match(pattern);
    if (match === null) {
      console.log('invalid command');
      return;
    }
    let command = match[1];
    let payload = match.length === 3 ? match[2] : '';
    try {
      let reply = parseCommand(command, payload);
      if (typeof reply === 'string') {
        console.log('reply: ' + reply);
        ws.send(reply);
      }
    } catch {
      console.log('reply: ' + reply);
      ws.send(reply);
    }
  });
});