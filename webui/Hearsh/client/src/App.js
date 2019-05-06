import React, { Component } from 'react';
import './App.css';

// My Components
import Header from './components/Header';
import ClockList from './components/ClockList';
import DisplayList from './components/DisplayList'

class App extends Component {
  constructor(props) {
    super(props);
    this.state = {
      message: 'Welcome to ClockIOT',
      clockIps: null,
      ipSelected: null,
      clockSocket: null,
      clockData: {
        'displays': [],
        'activeDisplay': null, 
      },
    };
  }

  changeMessage = (message) => {
    this.setState({
      message,
    });
  }

  changeIps = (clockIps) => {
    this.setState({
      clockIps,
    });
  }

  changeClockDisplays = (displays, activeDisplay) => {
    this.setState({
      clockData: {
        displays,
        activeDisplay,
      }
    })
  }

  handleChangeOfIp = (event) => {
    let ipSelected = event.target.value;
    if(ipSelected !== '') {
      this.setState({
        ipSelected,
      }, () => this.createWebSocket());
    } else {
      this.changeWebSocketUrl(null);
    }
  }

  checkForJson = (str) => {
    if(typeof str !== 'string') {
      return false;
    }
    let json = false;
    try{
      json = JSON.parse(str);
    } catch(err) {
      return false;
    }
    return json;
  }

  createWebSocket = () => {
    let url = `ws://${this.state.ipSelected}:81/`;
    try {
      let clockSocket = new WebSocket(url);
      this.setState({
        clockSocket,
      }, () => this.connectWebSocket(clockSocket));
    } catch(err) {
      console.log(err);
    }
  }

  connectWebSocket = () => {
    this.state.clockSocket.onopen = () => {
      console.log(`Clock IP changed to ${this.state.ipSelected}`);
      this.sendMessagetoSocket('get_displays');
    };
  }

  getSocketData = () => {
    this.state.clockSocket.onmessage = (message) => {
      let jsonData = this.checkForJson(message.data);
      if(jsonData.displays) {
        this.changeClockDisplays(jsonData.displays, jsonData.display_idx);
      }
    };
  }

  sendMessagetoSocket = (message, payload=null) => {
    console.log(message, payload);
    let data = `clockiot/${message}`;
    if (payload) {
      data += `//${payload}`;
    }
    this.state.clockSocket.send(data);
    this.getSocketData();
  }

  getIpData = () => {
    fetch('/getIpData').then(response => {
      response.json().then(data => {
        this.changeMessage(data.message);
        this.changeIps(data.data);
      })
    });
  }

  componentDidMount() {
    this.getIpData();
  }

  render() {
    return(
      <div>

        <Header
          message = {this.state.message}
        />

        {this.state.clockIps &&
          <ClockList
            ips = {this.state.clockIps}
            handleChangeOfIp = {this.handleChangeOfIp}
          />
        }

        {this.state.clockData.activeDisplay &&
          <DisplayList
            displays = {this.state.clockData.displays}
            sendMessage = {this.sendMessagetoSocket}
          />
        }

      </div>
    )
  }
}

export default App;
