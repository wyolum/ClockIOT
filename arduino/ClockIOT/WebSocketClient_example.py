#!/usr/bin/env python

# WS client example

import asyncio
import websockets

async def hello():
    async with websockets.connect('ws://192.168.1.184:81') as websocket:
        name = input("What's your name? ")

        await websocket.send(name)
        print(f"> {name}")

        greeting = await websocket.recv()
        print(f"< {greeting}")

async def clockiot_test1():
    async with websockets.connect('ws://192.168.1.183:81') as websocket:
        await websocket.send("clockiot/brighter//xxx")
        greeting = await websocket.recv()
        print(f"< {greeting}")
        
async def clockiot_test2():
    async with websockets.connect('ws://192.168.1.183:81') as websocket:
        await websocket.send("clockiot/dimmer")
        greeting = await websocket.recv()
        print(f"< {greeting}")

asyncio.get_event_loop().run_until_complete(clockiot_test1())
asyncio.get_event_loop().run_until_complete(clockiot_test2())

#for i in range(20):
#    asyncio.get_event_loop().run_until_complete(clockiot_test2())

