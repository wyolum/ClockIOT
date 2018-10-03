import tkinter
import asyncio
import websockets

async def send_msg(msg):
    async with websockets.connect('ws://192.168.1.183:81') as websocket:
        await websocket.send(msg)
        greeting = await websocket.recv()
        print(f"< {greeting}")

def brighter():
    asyncio.get_event_loop().run_until_complete(send_msg('clockiot/brighter'))
def dimmer():
    asyncio.get_event_loop().run_until_complete(send_msg('clockiot/dimmer'))
def send_mqtt_ip():
    bytes = []
    for i in range(4):
        bytes.append(mqtt_ip[i].get())
    msg = 'clockiot/mqtt_ip//' + '.'.join(bytes)
    print (msg)

root = tkinter.Tk()
frame = tkinter.Frame(root)
tkinter.Button(frame, text="Brighter", command=brighter).pack(side=tkinter.LEFT)
tkinter.Button(frame, text="Dimmer", command=dimmer).pack(side=tkinter.LEFT)
frame.pack()

mqtt_ip = []
mqtt_frame = tkinter.Frame(root)
tkinter.Button(mqtt_frame, text="Update MQTT IP:", command=send_mqtt_ip).pack(side=tkinter.LEFT)
bytes = ['192', '168', '1', '123']
for i in range(4):
    e = tkinter.Entry(mqtt_frame, width=3)
    e.insert(0, bytes[i])
    mqtt_ip.append(e)
    e.pack(side=tkinter.LEFT)
    if i < 3:
        tkinter.Label(mqtt_frame, text=".").pack(side=tkinter.LEFT)
mqtt_frame.pack()
root.mainloop()

