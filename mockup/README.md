# ClockIOT mockup
A mockup for clockiot built in NodeJS with a mini client built in svelte.

# Setup

## Requirements
To run the mockup it is required to have NodeJS and npm installed.

## Installing
First run `npm install` inside the mockup directory to install all dependencies. To setup the client, you need to run `npm run-script build` to compile the client into bundles. You only need to do these steps once.

## Running
To run just the server, run `npm run-script server`. The server can run on it's own, but there is no easy way to see the internal state without the client. To run the client simply run `npm start` or `npm run-script client`. `npm run-script client` will automatically build the client before serving, whereas `npm start` will only serve the client folder.

# Technical details.
The server is in the [server folder](./server) and the client is in the [src folder](./src).

## Server
The server uses [better-sqlite3](https://github.com/JoshuaWise/better-sqlite) and [enmap](https://github.com/eslachance/enmap) to store data, [ws](https://github.com/websockets/ws) to imitate the clock and send data to the debug client, and [request](https://github.com/request/request) with [request-promise-native](https://github.com/request/request-promise-native) to register as a clock with wyolum's servers.

The server mimics a clock by registering itself with wyolum's servers with it's own localip. By default it is localhost:81, but you can modify the [settings.json](./server/settings.json) file to change it's localip, port, faceplates, and displays.

### Debug mechanism
The client connects to the server by sending a simple `debug` message through the websocket connection, registering it as a debug client. The server automatically sends an object to the debug client with all the properties in it's database every 2 seconds. The client can also send a `debugOnce` command, which just sends the data once immediately.

## Client
The client uses [svelte](https://svelte.dev) as a framework and [sirv-cli](https://github.com/lukeed/sirv) to serve the static files.

The client will display all the properties of the fake clock and update as they change. This only happens every 2 seconds (see [Server](#server)), but can be refreshed immediately with a button at the bottom.