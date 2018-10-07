import tkinter
import asyncio
import websocket
import glob

root = tkinter.Tk()

ports = glob.glob('/dev/cu.SLAB*')
if len(ports) == 0:
    ports = ['No port available']

ESP32_IP = [192,168,1,183]
esp32_vars = [tkinter.IntVar() for i in range(4)]
[esp32_vars[i].set(ESP32_IP[i]) for i in range(4)]

ws = websocket.WebSocket(timeout=1)
ws.settimeout(1)

def hello():
    print ("hello")
def send_msg(msg):
    port = '81'
    ESP32_IP = [var.get() for var in esp32_vars]
    ip_str = 'ws://%s:%s/' % ('.'.join(map(str, ESP32_IP)), port)
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
portsmenu = tkinter.Menu(menubar, tearoff=0)
toolsmenu.add_cascade(label=ports[0], menu=portsmenu)
for i in range(5):
    portsmenu.add_command(label=str(i), command=hello)
    
menubar.add_cascade(label="Tools", menu=toolsmenu)

# display the menu
root.config(menu=menubar)

MODES = [
        ("Serial", "ser"),
        ("WiFi WebSocket", "ws"),
    ]

v = tkinter.StringVar()
v.set("S") # initialize
master = tkinter.Frame(root)
for text, mode in MODES:
    b = tkinter.Radiobutton(master, text=text, variable=v, value=mode)
    b.pack(anchor=tkinter.W)
master.pack()

esp32_frame = tkinter.Frame(root)
tkinter.Label(esp32_frame, text="ESP32 IP:").pack(side=tkinter.LEFT)
esp32_ip = []
for i in range(4):
    e = tkinter.Entry(esp32_frame, text=str(ESP32_IP[i]), textvariable=esp32_vars[i], width=3)
    # e.insert(0, ESP32_IP[i])
    esp32_ip.append(e)
    e.pack(side=tkinter.LEFT)
    if i < 3:
        tkinter.Label(esp32_frame, text=".").pack(side=tkinter.LEFT)
esp32_frame.pack()

frame = tkinter.Frame(root)
tkinter.Button(frame, text="Brighter", command=brighter).pack(side=tkinter.LEFT)
tkinter.Button(frame, text="Dimmer", command=dimmer).pack(side=tkinter.LEFT)
tkinter.Button(frame, text="Flip", command=flip_display).pack(side=tkinter.LEFT)
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

