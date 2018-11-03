from PIL import Image
import colorsys
import math
from math import cos, sin
from numpy import *

pi = 3.14159

if __name__ == "__main__":
    
    im = Image.new("RGB", (1000,1000))
    radius = min(im.size)/2.0
    centre = im.size[0]/2, im.size[1]/2
    pix = im.load()

    for r in range(450, 500):
        for theta_deg in arange(0, 360, .05):
            theta = theta_deg * pi / 180.
            x = r * cos(theta) + centre[0]; y = r * sin(theta) + centre[1]
            rx = x - centre[0]
            ry = y - centre[1]
            s = ((x - centre[0])**2.0 + (y - centre[1])**2.0)**0.5 / radius
            if s <= 1.0:
                h = ((math.atan2(ry, rx) / math.pi) + 1.0) / 2.0
                rgb = colorsys.hsv_to_rgb(h, 1, 1.0)
                pix[x,y] = tuple([int(round(c*255.0)) for c in rgb])
    fn = "colorwheel.png"
    im.save(fn)
    print("wrote", fn)
    im.show()
