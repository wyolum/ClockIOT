from PIL import Image
from pylab import *
from numpy import *

S = 1024
R = S // 2


def wheel(theta):
    '''
    Input a value 0 to 255 to get a color value.
    The colours are a transition r - g - b - back to r.
    '''
    theta = (theta) % (2 * pi)
    wheelpos = theta * 255 / (2 * pi)
    wheelpos = where(wheelpos > 255, 255, wheelpos)

    r1 = wheelpos * 3
    g1 = 255 - wheelpos * 3
    b1 = zeros(len(wheelpos))
    c1 = vstack([r1, g1, b1]).T

    r2 = 255 - wheelpos * 3
    g2 = zeros(len(wheelpos))
    b2 = wheelpos * 3
    c2 = vstack([r2, g2, b2]).T

    r3 = zeros(len(wheelpos))
    g3 = wheelpos * 3
    b3 = 255 - wheelpos * 3
    c3 = vstack([r3, g3, b3]).T

    out = c1.astype(uint8)
    step1 = 256 / 3.
    step2 = 256 - step1
    idx1 = wheelpos >= step1
    idx2 = wheelpos > step2
    out[idx1] = c2[idx1].astype(int)
    out[idx2] = c3[idx2].astype(int)
    return out


xs, ys = meshgrid(arange(S) - R, arange(S) - R)
x = xs.ravel()
y = ys.ravel()
r = sqrt(x ** 2 + y ** 2)
t = arctan2(y, x)

c = wheel(t)
alpha = ones(len(c), uint8) * 255
alpha[r > R] = 0
alpha[r < 2 * R / 4.5] = 0
rgba = dstack([c[:,0], c[:,1], c[:,2], alpha])
rgba = rgba.reshape((S, S, 4))
im = Image.fromarray(rgba, 'RGBA')
png = 'ColorWheel.png'
im.save(png)
print('Wrote', png)

im.show()
