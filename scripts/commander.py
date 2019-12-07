import tkinter
import glob
import serial
import serial.tools.list_ports

def get_serial_ports():
    ### mac
    ports = glob.glob('/dev/cu.*')

    ### ubuntu
    ports += glob.glob('/dev/ttyUSB*')
    ports = serial.tools.list_ports.comports()
    return [p.device for p in ports]

def curry(f, *args, **kw):
    def out():
        return f(*args, **kw)
    return out
        
PORT = [None]
CONNECTION = [None]

def select_port(port):
    PORT[0] = port
    CONNECTION[0] = serial.Serial(PORT[0], baudrate=115200, timeout=1)
    print('Port switched to "%s"' % PORT[0])
    print(CONNECTION[0])
    
def noop(*args, **kw):
    pass

def make_menu(tk):
    menubar = tkinter.Menu(tk)
    portmenu = tkinter.Menu(menubar, tearoff=0)
    for port in get_serial_ports():
        portmenu.add_command(label=port, command=curry(select_port, port))
    menubar.add_cascade(label="Ports", menu=portmenu)

    # display the menu
    tk.config(menu=menubar)

def readline():
    out = []
    if CONNECTION[0]:
        while True:
            c = CONNECTION[0].read(1)
            if c == '\n':
                break
            out.append(c)
    return ''.join(out)

def send_cmd(cmd, arg=''):
    if CONNECTION[0]:
        cmd = 'clockiot/%s//%s' % (cmd, arg)
        print(cmd)
        CONNECTION[0].write(cmd.encode('ascii'))
        # return readline()

def set_time():
    import time
    utc_offset = time.localtime().tm_gmtoff
    now = int(time.time() + utc_offset)
    cmd = 'set_time'
    arg = str(now)
    send_cmd(cmd, arg)
    
tk = tkinter.Tk()
make_menu(tk)
frame = tkinter.Frame(tk)
tkinter.Button(frame, text='Prev', command=curry(send_cmd, 'prev_display')).pack(side=tkinter.LEFT)
tkinter.Button(frame, text='Next', command=curry(send_cmd, 'next_display')).pack(side=tkinter.LEFT)
frame.pack()
frame = tkinter.Frame(tk)
tkinter.Button(frame, text='Brighter', command=curry(send_cmd, 'brighter')).pack(side=tkinter.LEFT)
tkinter.Button(frame, text='Dimmer', command=curry(send_cmd, 'dimmer')).pack(side=tkinter.LEFT)
frame.pack()
frame = tkinter.Frame(tk)
tkinter.Button(frame, text='Set PC Time', command=set_time).pack(side=tkinter.LEFT)
tkinter.Button(frame, text='Set NPT Time', command=curry(send_cmd, 'use_ntp')).pack(side=tkinter.LEFT)
frame.pack()
tk.mainloop()
