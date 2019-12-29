import numpy as np
import pylab as pl

WIDTH = 32
HEIGHT = 8

ciot = []
for i in range(8):
    row = np.arange(16)
    if i % 2 == 1:
        row = row[::-1]
    ciot.append(row)
ciot = np.array(ciot) + np.arange(8)[::-1][:,np.newaxis] * 16
chromogram = np.hstack([ciot + 128, ciot])
#print(chromogram)

#pl.pcolormesh(chromogram)
#pl.show()
#here
print("""\
// if MATRIXMAP_H is not already defind in MatrixMaps.h, cut and paste into ../libraries/MatrixMap/MatrixMap.h
#ifdef MATRIXMAP_H
#define MatrixWidth 32
#define MatrixHeight 8
uint16_t MatrixMap[MatrixHeight][MatrixWidth] = {""")
for i in range(8):
    print(" {",end='')
    for j in range(32):
        print ('%3d' % chromogram[i,j], end=',')
    print('},')
print('};')
print("#endif")
