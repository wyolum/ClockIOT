print("""\
// if CLOCKIOT is not already defind in MatrixMaps.h, cut and paste into ../libraries/MatrixMap/MatrixMap.h
#ifdef CLOCKIOT
#define MatrixWidth 16
#define MatrixHeight 8
uint16_t MatrixMap[MatrixHieght][MatrixWidth] = {""")
for i in range(8):
    print(" {",end='')
    for j in range(16):
        print ('%3d' % (16 * i +  (i % 2) * (15 - j) + ((i + 1) % 2) * j), end=', ')
    print('},')
print('};')
print("#endif")
