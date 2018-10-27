import urllib.request
import json
import tkinter
import asyncio
import websocket
import glob
import re

root = tkinter.Tk()

ESP32_IP = [192,168,1,183]
esp32_vars = [tkinter.IntVar() for i in range(4)]
[esp32_vars[i].set(ESP32_IP[i]) for i in range(4)]

ws = websocket.WebSocket(timeout=1)
ws.settimeout(1)

def hello():
    print ("hello")

def send_msg(msg):
    port = '81'
    ESP32_IP = esp32_ip.get()
    ip_str = 'ws://%s:%s/' % (ESP32_IP, port)
    print(ip_str)
    try:
        ws.connect(ip_str)
    except:
        print("FAILED")
        return
    ws.send(msg)
    greeting1 = ws.recv()
    # greeting2 = ws.recv()
    result = ws.close()
    print(f"< {greeting1}")
    # print(f"< {greeting2}")
def brighter():
    send_msg('clockiot/brighter')
def dimmer():
    send_msg('clockiot/dimmer')
def send_mqtt_ip():
    bytes = []
    for i in range(4):
        bytes.append(mqtt_ip[i].get())
    msg = 'clockiot/mqtt_ip//' + '.'.join(bytes)
    send_msg(msg)

def flip_display():
    send_msg('clockiot/flip_display')
    
def next_display():
    send_msg('clockiot/next_display')
    

menubar = tkinter.Menu(root)

# create a pulldown menu, and add it to the menu bar
filemenu = tkinter.Menu(menubar, tearoff=0)
filemenu.add_command(label="Open", command=hello)
filemenu.add_command(label="Save", command=hello)
filemenu.add_separator()
filemenu.add_command(label="Exit", command=root.quit)
menubar.add_cascade(label="File", menu=filemenu)

# create more pulldown menus
editmenu = tkinter.Menu(menubar, tearoff=0)
editmenu.add_command(label="Cut", command=hello)
editmenu.add_command(label="Copy", command=hello)
editmenu.add_command(label="Paste", command=hello)
menubar.add_cascade(label="Edit", menu=editmenu)

toolsmenu = tkinter.Menu(menubar, tearoff=0)
    
menubar.add_cascade(label="Tools", menu=toolsmenu)

# display the menu
root.config(menu=menubar)

page = urllib.request.urlopen('http://www.wyolum.com/utc_offset/get_localips.py')
txt = page.read().decode('utf-8')
localips = json.loads(txt)['localips']

esp32_ip = tkinter.StringVar()

have = []
for localip in localips:
    if localip and localip not in have:
        ip = localip["localip"]
        t = localip["type"]
        esp32_ip.set(ip)
        b = tkinter.Radiobutton(root, text="%s-%s" % (ip, t),
                                variable=esp32_ip, value=ip)
        b.pack(anchor=tkinter.W)
        have.append(localip)
    

frame = tkinter.Frame(root)
tkinter.Button(frame, text="Brighter", command=brighter).pack(side=tkinter.LEFT)
tkinter.Button(frame, text="Dimmer", command=dimmer).pack(side=tkinter.LEFT)
tkinter.Button(frame, text="Flip", command=flip_display).pack(side=tkinter.LEFT)
tkinter.Button(frame, text="Next", command=next_display).pack(side=tkinter.LEFT)
frame.pack()

mqtt_ip = []
mqtt_frame = tkinter.Frame(root)
tkinter.Button(mqtt_frame, text="Update MQTT IP:", command=send_mqtt_ip).pack(side=tkinter.LEFT)
bytes = ['192', '168', '1', '159']
for i in range(4):
    e = tkinter.Entry(mqtt_frame, width=3)
    e.insert(0, bytes[i])
    mqtt_ip.append(e)
    e.pack(side=tkinter.LEFT)
    if i < 3:
        tkinter.Label(mqtt_frame, text=".").pack(side=tkinter.LEFT)
mqtt_frame.pack()
root.mainloop()

