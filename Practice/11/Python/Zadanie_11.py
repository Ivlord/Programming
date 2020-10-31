
def Z_11a():
    print('\n\n==================================================')
    print(    '= Возведение в степень с использованием рекурсии =')
    print(    '==================================================\n')

    base  =SecureInput([ [float] ], separ=' ', hlp='Введите вещественное число: ', clr='')[0]
    stepen=SecureInput([ [int] ]  , separ=' ', hlp='Введите его степень (целое число): ', clr='')[0]
    print(FloatToIntNoLoseCheck(StepenChisla(base, stepen)))

def StepenChisla(base, stepen):
    return base*StepenChisla(base, stepen-1) if stepen else 1

def Z_11b():
    print('\n\n============================================================')
    print(    '= Возведение в степень традиционные варианты с while и for =')
    print(    '============================================================\n')

    base  =SecureInput([ [float] ], separ=' ', hlp='Введите вещественное число: ', clr='')[0]
    stepen=SecureInput([ [int] ]  , separ=' ', hlp='Введите его степень (целое число): ', clr='')[0]

    res=1
    for i in range(stepen):
        res*=base
    print('\nс for: ', FloatToIntNoLoseCheck(res))

    res=1
    while stepen:
        res*=base
        stepen-=1
    print('\nс while: ', FloatToIntNoLoseCheck(res))


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
    Z_11a()
    Z_11b()
