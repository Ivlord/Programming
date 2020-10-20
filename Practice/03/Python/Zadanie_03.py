#
def Zadanie_03():
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

    print('Сумма        a + b =', var[0] + var[1], ' '*5, type(var[0]), '+', type(var[1]), '=', type(var[0] + var[1]))
    print('Разница      a - b =', var[0] - var[1], ' '*5, type(var[0]), '-', type(var[1]), '=', type(var[0] - var[1]))
    print('Произведение a * b =', var[0] * var[1], ' '*5, type(var[0]), '*', type(var[1]), '=', type(var[0] * var[1]))
    print('Деление      a / b =', var[0] / var[1], ' '*5, type(var[0]), '/', type(var[1]), '=', type(var[0] / var[1]))

Zadaniye_03()
