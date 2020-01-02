import csv
from spreadsheet import Spreadsheet
from numpy import nan, inf, zeros

def readwtf(csvfile, n_row=8, n_col=16, printit=True):
    if printit:
        pass
    else:
        def print(*args, **kw):
            pass
    f = csv.reader(open(csvfile))
    lines = list(f)
    ss = Spreadsheet(lines)
    ## check WTF
    assert ss.parsecell('C2')[0] == 1
    assert ss.parsecell('C2')[1] == 2
    assert ss.getRegion('C1')[0][0] == '0'
    assert ''.join(ss.getCell('U1').lower().split()) == 'startrow'
    assert ''.join(ss.getCell('U2').lower().split()) == 'startcol'
    assert ''.join(ss.getCell('U3').lower().split()) == 'length'
    
    letters = ss.getRegion('C2:R9')
    letters = [[c.lower() for c in row] for row in letters]
    
    print('\n'.join([''.join(l) for l in letters]))
    rows = list(map(int, ss.getRegion('V1:*1')[0]))
    n_word = len(rows)
    cols = list(map(int, ss.getRegion('V2:*2')[0]))
    lens = list(map(int, ss.getRegion('V3:*3')[0]))
    words = ss.getRegion('V4:*4')[0][:n_word]
    assert len(words) == len(rows), '%s != %s' % (len(words), len(rows))
    bitmap = zeros((288, n_word), int)
    dat = ss.getRegion('V6:*294')
    printit = True
    for i in range(288):
        if printit:
            print('%02d:%02d' % (i / 12, (5 * i) % 60), end=' ')
        for j in range(n_word):
            if (j < len(dat[i]) and
                dat[i][j] is not None and 
                dat[i][j].strip() != ''):
                if printit:
                    print(words[j], end=' ')
                bitmap[i, j] = 1
        if printit:
            print()
            
    min_rows = []
    min_cols = []
    n_min_led = 0
    n_min_state = 0
    min_bitmap = None
    n_min_led = ss.getCell('V294')
    if n_min_led is not None:
        n_min_led = int(n_min_led)
        n_min_state = int(ss.getCell('X294'))
        if n_min_led > 0:
            min_rows = list(map(int, ss.getRegion('V295:*296')[0][:n_min_led]))
            min_cols = list(map(int, ss.getRegion('V296:*296')[0][:n_min_led]))
            i, j = ss.parsecell('V297')
            cells = ss._getRegion(i, j, n_min_state, n_min_led)
            min_bitmap = zeros((n_min_state, n_min_led), int)
            for i in range(n_min_state):
                if i < len(cells):
                    l = cells[i]
                else:
                    break
                for j in range(n_min_led):
                    if j < len(l):
                        c = l[j]
                        if c is not None and c.strip() != '':
                            min_bitmap[i, j] = 1
                    else:
                        break

    author = ss.getCell('B20')
    email = ss.getCell('B21')
    licence = ss.getCell('B22')
    desc = ss.getCell('B23')
    return {'letters': letters,
            'data':bitmap, 
            'rows':rows,
            'cols':cols,
            'lens':lens,
            'words':words,
            'min_rows':min_rows,
            'min_cols':min_cols,
            'n_min_led': n_min_led,
            'n_min_state': n_min_state,
            'min_bitmap': min_bitmap,
            'author':author,
            'email':email,
            'licence':licence,
            'desc':desc,
            'filename':csvfile,
            }

def readcsv(csvfile, n_row=8):
    f = csv.reader(open(csvfile))
    letters = [next(f) for i in range(n_row)]
    rows = next(f)
    cols = next(f)
    lens = next(f)
    words = f.next()[1:]
    n_word = len(words)
    bitmap = zeros((288, n_word), int)
    for i in range(288):
        l = next(f)
        for j, c in enumerate(l[1:]):
            if c:
                bitmap[i, j] = 1
    minutes_hack = list(f)
    if len(minutes_hack):
        min_rows = list(map(int, minutes_hack[0][1:]))
        min_cols = list(map(int, minutes_hack[1][1:]))
        n_min_led = min([len(min_rows),len(min_cols)])
        n_min_state = len(minutes_hack) - 2
        min_bitmap = zeros((n_min_state, n_min_led), int)
        for i, l in enumerate(minutes_hack[2:]):
            for j, v in enumerate(l[1:]):
                if v.strip() != '':
                    min_bitmap[i, j] = 1
        
    else:
        min_rows = []
        min_cols = []
        n_min_led = 0
        n_min_state = 0
        min_bitmap = None
    return {'letters': letters,
            'data':bitmap, 
            'rows':list(map(int, rows[1:])),
            'cols':list(map(int, cols[1:])),
            'lens':list(map(int, lens[1:])),
            'words':words[1:],
            'min_rows':min_rows,
            'min_cols':min_cols,
            'n_min_led': n_min_led,
            'n_min_state': n_min_state,
            'min_bitmap': min_bitmap,
            }

