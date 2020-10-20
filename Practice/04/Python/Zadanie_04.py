
def Zadanie_04():
    print('Введите по очереди (через Enter) две переменные: ')
    dat, var = [['a', 'перв'], ['b', 'втор']], [0, 0]
    for i in range(2):
        while True:
            var[i] = input('Введите ' + dat[i][1] + 'ую переменную: ')
            try:
                if '.' in var[i]: var[i] = float(var[i])
                else:             var[i] = int(var[i])
            except:
                print(dat[i][1].title() + 'ая переменная введена не правильно. повторите ввод.\n')
                continue
            else:
                print(' целое' if isinstance(var[i], int) else ' вещественное' , dat[i][0], '=', var[i], '\n')
                break
    a, b = var

    print('== Обмен c участием третьей переменной ==\n')
    c = a
    a = b
    b = c
    print('Через 3-тью переменную:  a =', a, ', b =', b, '\n')

    print('== Обмен без участия третьей переменной ==\n')
    a, b = b, a
    print('Вариант 1(главный для Python):  a =', a, ', b =', b, '\n')

    if isinstance(a, int) and isinstance(b, int):
        a = a ^ b
        b = a ^ b
        a = a ^ b
        print('Вариант 2(XOR):  a =', a, ', b =', b, ' Не подходит, если одна из переменных имеет тип float.\n')
    else:
        print('Вариант 2 с XOR неприменим, т.к. учавствует переменная float типа.\n')

    a+= b
    b = a - b
    a = a - b
    print('Вариант 3(сумма):  a =', a, ', b =', b, ' Неприменим при больших числах.\n')

    a*= b
    b =a / b
    a =a / b
    print('Вариант 4(умножение):  a =', a, ', b =', b, ' Неприменим при больших числах. При таком варианте обе переменные будут иметь после обмена тип float.')



Zadanie_04()
