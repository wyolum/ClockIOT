// brighter dimmer flip next color
document.addEventListener("DOMContentLoaded", () => {
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
  };

  function loadingState(state) {
    // change loading state
    var loadingSpinner = document.querySelector("#loadingSpinner");
    var controls = document.querySelector("#controls");
    switch (state) {
      case "none":
        loadingSpinner.classList.add("hidden");
        controls.classList.add("hidden");
        break;
      case "loading":
        loadingSpinner.classList.remove("hidden");
        controls.classList.add("hidden");
        break;
      case "connected":
        loadingSpinner.classList.add("hidden");
        controls.classList.remove("hidden");
        break;
    }
  }
  loadingState("loading");

  async function refreshDropdown() {
    // get clock list from api
    var clockList = await fetch("/utc_offset/get_localips.py")
      .then(response => {
        return response.json();
      })
      .then(json => {
        return json.localips;
      });

    var listElement = document.querySelector("#clockList");

    // remove previous elements
    while (listElement.length > 1) {
      listElement.remove(1);
    }

    // add detected clockiots
    for (let clock of clockList) {
      if (clock.dev_type === "ClockIOT") {
        let listItem = new Option(clock.localip, clock.localip);
        listElement.add(listItem);
      }
    }

    loadingState("none");

    document.querySelector("#clockListPlaceholder").innerHTML =
      "Select your ClockIOT";

    if (listElement.length > 1) {
      listElement.selectedIndex = 1;
      changeClock(listElement.value);
    }
  }

  refreshDropdown();

  function checkSocket(socket) {
    if (typeof socket !== "undefined" && socket instanceof WebSocket) {
      return true;
    } else {
      console.error("No clock selected");
      alert("Choose a ClockIOT");
      return false;
    }
  }

  function changeClock(ip) {
    if (ip === "") {
      // Choose a ClockIOT option
      clockSocket.close();
      loadingState("none");
      return;
    }
    if (clockSocket instanceof WebSocket) {
      // if previous websocket open, close it
      clockSocket.close();
    }

    url = `ws://${ip}:81/`;
    try {
      loadingState("loading");
      clockSocket = new WebSocket(url);
    } catch (error) {
      if (
        error.code === DOMException.SECURITY_ERR &&
        location.protocol === "https:"
      ) {
        location.protocol = "http:";
        console.error(
          "Browsers require websockets to be secure on https pages. Redirecting..."
        );
      }
    }

    clockSocket.onopen = () => {
      console.log("Clock IP changed to " + ip);
      generateDisplayList();
      generateLanguageList();
    };

    clockSocket.onmessage = message => {
      var data = message.data;
      if (data.charAt(0) !== "{") return;
      data = JSON.parse(data);
      if (typeof data.displays !== "undefined") {
        generateDisplayList(data.displays, data.display_idx);
        return;
      } else if (typeof data.faceplates !== "undefined") {
        generateLanguageList(data.faceplates, data.faceplate_idx);
      }
    };

    clockSocket.onerror = error => {
      console.error(error);
    };

    clockSocket.onclose = () => {
      console.log("Clock socket closing");
    };
  }

  function sendMessage(message, payload) {
    if (!checkSocket(clockSocket)) {
      return false;
    }

    var data = `clockiot/${message}`;
    if (typeof payload !== "undefined") {
      data += `//${payload}`;
    }
    console.log(data);
    clockSocket.send(data);
    return true;
  }

  function generateDisplayList(displays, activeDisplay) {
    if (typeof displays === "undefined") {
      sendMessage("get_displays");
      return;
    }

    activeDisplay = parseInt(activeDisplay);

    var listElement = document.querySelector("#displayList");
    while (listElement.length > 0) {
      // clear list
      listElement.remove(0);
    }

    for (let [id, name] of displays.entries()) {
      let selected = id === activeDisplay;
      let option = new Option(name, id, selected, selected);
      listElement.add(option);
    }
    loadingState("connected");
  }

  function setDisplay(displayID) {
    sendMessage("display_idx", displayID);
  }

  function setColor(color) {
    // payload: rrggbb lowercase html color code example "ff0000" is RED
    sendMessage("set_rgb", color);
  }

  function generateLanguageList(languages, activeLanguage) {
    if (typeof languages === "undefined") {
      sendMessage("get_faceplates");
      return;
    }

    activeLanguage = parseInt(activeLanguage);

    var listElement = document.querySelector("#languageList");
    while (listElement.length > 0) {
      // clear list
      listElement.remove(0);
    }

    window.languages = languages;
    window.activeLanguage = activeLanguage;
    for (let [id, dirtyName] of languages.entries()) {
      // clean version numbers off of languages
      let name = options.showFaceplateVersions
        ? dirtyName
        : cleanName(dirtyName);
      let selected = id === activeLanguage;
      let option = new Option(name, id, selected, selected);
      let nextName = cleanName(languages[id + 1]);
      if (options.showFaceplateVersions || selected || name !== nextName) {
        // only use latest version of faceplate unless showFaceplateVersions true
        // or it's already selected
        listElement.add(option);
      }
    }
  }

  function cleanName(name) {
    // turn name from English_V3 into English
    if (typeof name === "undefined") return ""; // for last item
    const matches = name.match(/^(.*)_V\d$/);
    if (!matches || !matches.length) return name;
    return matches[1];
  }

  function setLanguage(faceplateID) {
    sendMessage("faceplate_idx", faceplateID);
  }

  function getBrowserOffset() {
    var offset = new Date().getTimezoneOffset();
    return -offset / 60;
  }

  function generateTimezoneList() {
    // grab elements
    var map = document.querySelector("#timezone-map");
    var list = document.querySelector("#timezone-list");
    var offsetField = document.querySelector("#offset-field");
    var confirmButton = document.querySelector("#changeTimezone");

    var ipTimeChoice = new Option("Auto Detect Timezone");
    ipTimeChoice.dataset.offset = "AUTO";
    list.add(ipTimeChoice);

    var browserTimeChoice = new Option("Browser Timezone", "");
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

    list.addEventListener("change", () => {
      transferOffsetToField();
    });

    function timezoneChangeHandler(timezoneName, countryName, offset) {
      transferOffsetToField(offset);
    }
    window.timezoneChangeHandler = timezoneChangeHandler;

    confirmButton.addEventListener("click", event => {
      // if setting timezone to use ip, ignore any numbered offset
      if (offsetField.value === "AUTO" || offsetField.value === "IP") {
        sendMessage("timezone_offset", "IP");
        return;
      }

      var offset = Number(offsetField.value);
      if (offset.toString() === "NaN") {
        console.error("Invalid offset " + offset);
        alert("Invalid offset");
        return false;
      }

      // convert UTC offset from hours to seconds
      var secondOffset = offset * 60 * 60;
      sendMessage("timezone_offset", secondOffset);
    });

    transferOffsetToField();
  }
  generateTimezoneList();

  function createListeners() {
    function clockListListener(event) {
      changeClock(event.target.value);
    }

    let clockList = document.querySelector("#clockList");
    clockList.addEventListener("change", clockListListener);

    function buttonListener(event) {
      // sends ws message with value attribute of button pressed
      sendMessage(event.target.value);
    }

    for (let button of document.querySelectorAll(".wsButton")) {
      button.addEventListener("click", buttonListener);
    }

    let displayList = document.querySelector("#displayList");

    displayList.addEventListener("change", event => {
      let displayIndex = displayList.selectedIndex;
      setDisplay(displayIndex);
    });

    let nextDisplayButton = document.querySelector("#nextDisplayButton");
    nextDisplayButton.addEventListener("click", event => {
      let displayIndex = displayList.selectedIndex;
      displayIndex++;
      if (displayIndex >= displayList.length) displayIndex = 0;
      displayList.selectedIndex = displayIndex;
      setDisplay(displayIndex);
    });

    let previousDisplayButton = document.querySelector(
      "#previousDisplayButton"
    );
    previousDisplayButton.addEventListener("click", event => {
      let displayIndex = displayList.selectedIndex;
      displayIndex--;
      if (displayIndex < 0) {
        displayIndex = displayList.length - 1;
      } else if (displayIndex >= displayList.length) {
        displayIndex = 0;
      }
      displayList.selectedIndex = displayIndex;
      setDisplay(displayIndex);
    });

    let languageList = document.querySelector("#languageList");
    languageList.addEventListener("change", event => {
      setLanguage(event.target.value);
    });

    const colorInput = document.querySelector("#colorInput");
    colorInput.addEventListener("change", event => {
      // console.log(event.target.value);
      let color = event.target.value;
      if (color.startsWith("#")) {
        color = color.replace("#", "");
      }
      setColor(color);
    });

    // $("#timezoneModal").on("shown.bs.modal", function(e) {
    //   $("#timezone-image").timezonePicker({
    //     changeHandler: window.timezoneChangeHandler,
    //     target: "#timezone-list"
    //   });
    // });
  }
  createListeners();
});
