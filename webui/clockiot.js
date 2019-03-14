// brighter dimmer flip next color
document.addEventListener('DOMContentLoaded', () => {
  // variables
  // websocket connection
  var clockSocket;
  // unread messages flag
  var unreadMessage = false;
  // connection state
  /* none
   * loading
   * error
   * connected
  */

  var options = {
    // options object
    showFaceplateVersions: false
  }

  function loadingState(state) {
    // change loading state
    var loadingSpinner = document.querySelector('#loadingSpinner');
    var controls = document.querySelector('#controls');
    switch (state) {
      case 'none':
        loadingSpinner.style.display = 'none';
        controls.classList.add('hidden');
        break;
      case 'loading':
        loadingSpinner.style.display = 'inline-block';
        controls.classList.add('hidden');
        break;
      case 'connected':
        loadingSpinner.style.display = 'none';
        controls.classList.remove('hidden');
        break;
    }
  }
  loadingState('none');

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
    if (ip === '') {
      // Choose a ClockIOT option
      clockSocket.close();
      loadingState('none');
      return;
    }
    if (clockSocket instanceof WebSocket) {
      // if previous websocket open, close it
      clockSocket.close();
    }

    url = `ws://${ip}:81/`;
    try {
      loadingState('loading');
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
      generateLanguageList();
    }

    clockSocket.onmessage = (message) => {
      var data = message.data;
      if (data.charAt(0) !== '{') return;
      data = JSON.parse(data);
      if(typeof data.displays !== 'undefined') {
        generateDisplayList(data.displays, data.display_idx);
        return;
      } else if (typeof data.faceplates !== 'undefined') {
        generateLanguageList(data.faceplates, data.faceplate_idx);
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

    activeDisplay = parseInt(activeDisplay);

    var listElement = document.querySelector('#displayList');
    while(listElement.length > 0) {
      // clear list
      listElement.remove(0);
    }

    for (let [id, name] of displays.entries()) {
      let selected = (id === activeDisplay);
      let option = new Option(name, id, selected, selected);
      listElement.add(option);
    }
    loadingState('connected');
  }

  function setDisplay(displayID) {
    sendMessage('display_idx', displayID);
  }

  function generateLanguageList(languages, activeLanguage) {
    if (typeof languages === 'undefined') {
      sendMessage('get_faceplates');
      return;
    }

    activeLanguage = parseInt(activeLanguage);

    var listElement = document.querySelector('#languageList');
    while(listElement.length > 0) {
      // clear list
      listElement.remove(0);
    }

    window.languages = languages
    window.activeLanguage = activeLanguage
    for (let [id, dirtyName] of languages.entries()) {
      // clean version numbers off of languages
      let name = options.showFaceplateVersions ? dirtyName : cleanName(dirtyName);
      let selected = (id === activeLanguage);
      let option = new Option(name, id, selected, selected);
      let nextName = cleanName(languages[id+1]);
      if(options.showFaceplateVersions || selected || name !== nextName) {
        // only use latest version of faceplate unless showFaceplateVersions true
        // or it's already selected
        listElement.add(option);
      }
    }
  }

  function cleanName(name) {
    // turn name from English_V3 into English
    if (typeof name === 'undefined') return ''; // for last item
    return name.match(/^(.*)_V\d$/)[1];
  }

  function setLanguage(faceplateID) {
    sendMessage('faceplate_idx', faceplateID);
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
  var buttonDiv = document.querySelector('#modalToggles');

  var timezoneModal = new Modal('#timezoneModal');
  var timezoneButton = timezoneModal.openButton();
  timezoneButton.id =  'timezoneButton';
  timezoneButton.innerText = 'Select Timezone';
  buttonDiv.appendChild(timezoneButton);

  var colorModal = new Modal('#colorPickerModal');
  var colorButton = colorModal.openButton();
  colorButton.id = 'colorButton';
  colorButton.innerText = 'Select Color';
  buttonDiv.appendChild(colorButton);

  // color picker
  var colorPicker = new ColorPicker(document.querySelector('#colorPicker'));

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

    displayList.addEventListener('change', (event) => {
      let displayIndex = displayList.selectedIndex;
      setDisplay(displayIndex);
    })

    let languageList = document.querySelector('#languageList');
    languageList.addEventListener('change', (event) => {
      setLanguage(event.target.value);
    });

    function pickerListener(event) {
      var color = event.detail.color;
      console.log(color);
      var output;
      switch (event.type) {
        case 'click':
          output = document.querySelector('#colorPickerTest').children[1];
          break;
        case 'mousemove':
          output = document.querySelector('#colorPickerTest').children[0];
          break;
      }
      output.style.backgroundColor = color;
    }
    colorPicker.addEventListener('click', pickerListener);
    colorPicker.addEventListener('mousemove', pickerListener);
  }
  createListeners();


});
