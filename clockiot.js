// brighter dimmer flip next color
document.addEventListener('DOMContentLoaded', () => {
  // variables
  // websocket connection
  var clockSocket;
  // unread messages flag
  var unreadMessage = false;

  async function refreshDropdown() {
    // get clock list from api
    var clockList = await fetch('https://www.wyolum.com/utc_offset/get_localips.py')
    .then(response => {
      return response.json();
    }).
    then(json => {
      return json.localips;
    });

    var listElement = document.querySelector('#clockList');

    // remove previous elements
    while (listElement.length > 1) {
      listElement.remove(1);
    }

    // add detected clockiots
    for (let clock of clockList) {
      if(clock.dev_type === 'ClockIOT') {
        let listItem = new Option(clock.localip, clock.localip);
        listElement.add(listItem);
      }
    }
  }
  refreshDropdown();

  function checkSocket(socket) {
    if (typeof socket !== 'undefined' && socket instanceof WebSocket) {
      return true;
    } else {
      console.error('No clock selected');
      alert('Choose a ClockIOT');
      return false;
    }
  }

  function changeClock(ip) {
    if (clockSocket instanceof WebSocket) {
      // if previous websocket open, close it
      clockSocket.close();
    }

    url = `ws://${ip}:81/`;
    try {
      clockSocket = new WebSocket(url);
    } catch(error) {
      if (error.code === DOMException.SECURITY_ERR && location.protocol === 'https:') {
        location.protocol = 'http:';
        console.error('Browsers require websockets to be secure on https pages. Redirecting...');
      }
    }

    clockSocket.onopen = () => {
      console.log('Clock IP changed to ' + ip);
      generateDisplayList();
    }

    clockSocket.onmessage = (message) => {
      var data = message.data;
      if (data.charAt(0) !== '{') return;
      data = JSON.parse(data);
      if(typeof data.displays !== 'undefined') {
        generateDisplayList(data.displays, data.display_idx);
        return;
      }
    };

    clockSocket.onerror = (error) => {
      console.error(error);
    }

    clockSocket.onclose = () => {
      console.log('Clock socket closing');
    }
  }

  function sendMessage(message, payload) {
    if (!checkSocket(clockSocket)) {
      return false;
    }

    var data = `clockiot/${message}`;
    if (typeof payload !== 'undefined') {
      data += `//${payload}`;
    }
    console.log(data);
    clockSocket.send(data);
    return true;
  }

  function generateDisplayList(displays, activeDisplay) {
    if (typeof displays === 'undefined') {
      sendMessage('get_displays');
      return;
    }
    var listElement = document.querySelector('#displayList');
    while(listElement.length > 0) {
      // clear list
      listElement.remove(0);
    }

    for (let [id, name] of displays.entries()) {
      let selected = (id === activeDisplay);
      let option = new Option(name, id, selected);
      listElement.add(option);
    }
  }

  function setDisplay(displayID) {
    sendMessage('display_idx', displayID);
  }

  function getBrowserOffset() {
    var offset = new Date().getTimezoneOffset();
    return -offset / 60;
  }

  function generateTimezoneList() {
    // grab elements
    var map = document.querySelector('#timezone-map');
    var list = document.querySelector('#timezone-list');
    var offsetField = document.querySelector('#offset-field');
    var confirmButton = document.querySelector('#changeTimezone');

    var ipTimeChoice = new Option('Auto Detect Timezone');
    ipTimeChoice.dataset.offset = 'AUTO';
    list.add(ipTimeChoice);

    var browserTimeChoice = new Option('Browser Timezone', '')
    browserTimeChoice.dataset.offset = getBrowserOffset();
    list.add(browserTimeChoice);


    for (let area of map.areas) {
      let timezone = area.dataset.timezone;
      let offset = area.dataset.offset;
      let option = new Option(timezone, timezone);
      option.dataset.offset = offset;

      list.add(option);
    }

    function transferOffsetToField(offset) {
      // offset is optional
      offsetField.value = offset || list.selectedOptions[0].dataset.offset;
    }

    list.addEventListener('change', () => {transferOffsetToField()});

    function timezoneChangeHandler(timezoneName, countryName, offset) {
      transferOffsetToField(offset);
    }
    window.timezoneChangeHandler = timezoneChangeHandler;

    confirmButton.addEventListener('click', event => {
      // if setting timezone to use ip, ignore any numbered offset
      if (offsetField.value === 'AUTO' || offsetField.value === 'IP') {
        sendMessage('timezone_offset', 'IP');
        return;
      }

      var offset = Number(offsetField.value);
      if (offset.toString() === 'NaN') {
        console.error('Invalid offset ' + offset);
        alert('Invalid offset');
        return false;
      }

      // convert UTC offset from hours to seconds
      var secondOffset = ((offset * 60) * 60);
      sendMessage('timezone_offset', secondOffset);
    });

    transferOffsetToField();
  }
  generateTimezoneList();


  // modals
  var timezoneModal = new Modal('#timezoneModal');
  var colorModal = new Modal('#colorPickerModal');

  function createListeners() {
    function clockListListener(event) {
      changeClock(event.target.value);
    }

    let clockList = document.querySelector('#clockList')
    clockList.addEventListener('change', clockListListener);

    function buttonListener(event) {
      // sends ws message with value attribute of button pressed
      sendMessage(event.target.value)
    }

    for (let button of document.querySelectorAll('.wsButton')) {
      button.addEventListener('click', buttonListener);
    }

    let displayList = document.querySelector('#displayList');
    let changeDisplay = document.querySelector('#changeDisplay');

    changeDisplay.addEventListener('click', (event) => {
      let displayIndex = displayList.selectedIndex;
      setDisplay(displayIndex);
    })

    let timezoneButton = document.querySelector('#timezoneButton');
    let colorButton = document.querySelector('#colorButton');

    timezoneButton.addEventListener('click', timezoneModal.open);
    colorButton.addEventListener('click', colorModal.open);
  }
  createListeners();


});
