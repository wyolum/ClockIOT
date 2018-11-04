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
    clockSocket = new WebSocket(url);

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
