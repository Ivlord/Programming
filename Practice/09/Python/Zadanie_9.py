# Встреча
def Z_9a():
    print('==========================================')
    print('= Без учета переноса на другой день      =')
    print('=   т.е. оба времени указаны в один день =')
    print('==========================================\n')
    t1=SecureInput([ [int, [0,23] ], [int, [0,59] ] ], separ=':', hlp='Введите первое время: ', clr='')
    t2=SecureInput([ [int, [0,23] ], [int, [0,59] ] ], separ=':', hlp='Введите второе время: ', clr='')
    print('Встреча ', 'не ' if abs(t1[0]*60+t1[1] - t2[0]*60-t2[1])>15 else '' ,'состоится', sep='')

def Z_9b():
    print('\n\n=====================================================================================')
    print('= С учетом переноса на другой день, т.е, время вводится в порядке следования        =')
    print('= шкалы времени: если второе время меньше, то считаем, что это уже следующие сутки. =')
    print('== Например: 23:55 и 0:05 ===========================================================\n')
    t1=SecureInput([ [int, [0,23] ], [int, [0,59] ] ], separ=':', hlp='Введите первое время: ', clr='')
    t2=SecureInput([ [int, [0,23] ], [int, [0,59] ] ], separ=':', hlp='Введите второе время: ', clr='')
    t1, t2 = t1[0]*60+t1[1], t2[0]*60+t2[1]
    print('Встреча ', 'не ' if abs(t1 - (t2+24*60 if t2<t1 else t2) )>15 else '' ,'состоится', sep='')


#
##################################################################################
#
def FloatToIntNoLoseCheck(x):
    return int(x) if not (x-int(x)) else x

# function   range  function   NO range, если любые числа
# a:[ [int, [1, 2],  [float] ]
def SecureInput(a, separ=' ', hlp='', clr='(),'):
    if not isinstance(a, list) or not a: return []
    while True:
        raw = input(hlp)
        for i in clr: raw = raw.replace(i, ' ')
        raw = raw.split() if separ==' ' else raw.split(separ)
        try:
            if len(raw)>len(a): raise Exception('   введены лишние данные\n')

            res=[a[i][0](raw[i]) for i in range(len(a))
                if len(a[i])==1 or a[i][1][0] <= a[i][0](raw[i]) <= a[i][1][1]]

            if len(res)<len(a): raise Exception('   ошибка диапазона\n') # что-то из данных вне диапазона
            break

        except LookupError:     print('   не все данные введены\n')
        except ValueError:      print('   ошибка типа данных.\n')
        except Exception as e:  print(e)
    return res


if __name__ == "__main__":
    Z_9a()
    Z_9b()
