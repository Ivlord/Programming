def Z_16():
    print('\n\n============================')
    print(    '= Проверка номеров билетов =')
    print(    '============================\n')
    letters='abcdefghijklmnopqrstuvwxyz'
    #        012345678
    shablon='a???55661'
    while True:
        n=SecureInput([ [int, [1,10**9]] ], hlp='Сколко билетов у Сигизмунды?: ', clr='')[0]
        bileti=input('Вводите номера (две буквы [a-z] и 7 цифер) через пробел:\n').split()
        if len(bileti)!=n:
            print('Что-то вы их слишком', 'мало' if len(bileti)<n else 'много', 'ввели. Попробуем всё заново!\n\n')
            continue

        brak= [i for i in bileti for j in [0,1] if i[j] not in letters or len(i)!=9]
        brak+=[i for i in bileti for j in range(2,9) if i[j] not in '0123456789']

        if brak:
            print('Эти номера введены не правильно!', list(set(brak)), '\nПопробуем всё заново!\n\n')
            continue

        palivo=[i for i in bileti if shablon==i[0]+'???'+i[4:]]
        print(' '.join(palivo) if palivo else -1)
        break




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
    Z_16()
