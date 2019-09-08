from PIL import Image
import colorsys
import math
from math import cos, sin
from numpy import *
import tkinter
import PIL
from PIL import ImageTK
pi = 3.14159

D = 300
wheel_fn = 'colorwheel.png'

if __name__ == "__main__":
    
    root = tkinter.Tk()
    can = tkinter.Canvas(root, width=D, height=D)
    wheel_im = Image.open(wheel_fn)
    wheel_ph = tkinter.ImageTk.PhotoImage(wheel_im)
    can.create_image(0, 0, wheel_ph)
    can.pack()
    root.mainloop()
    here
    radius = D / 2.0
    centre = radius, radius

    for r in range(120, 150):
        for theta_deg in arange(0, 360, .15):
            theta = theta_deg * pi / 180.
            x = r * cos(theta) + centre[0]; y = r * sin(theta) + centre[1]
            rx = x - centre[0]
            ry = y - centre[1]
            s = ((x - centre[0])**2.0 + (y - centre[1])**2.0)**0.5 / radius
            if s <= 1.0:
                h = ((math.atan2(ry, rx) / math.pi) + 1.0) / 2.0
                rgb = colorsys.hsv_to_rgb(h, 1, 1.0)
                pix[x,y] = tuple([int(round(c*255.0)) for c in rgb])
    im.show()
