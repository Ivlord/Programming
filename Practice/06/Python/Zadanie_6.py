
# Задание 6. Квадратное уравнение.
def Z_6(): #
    while True:
        print('Введите a,b,c - вещественные числа:')
        try:
            a = float(input('a> '))
            b = float(input('b> '))
            c = float(input('c> '))
            break

        except:
            print('   ошибка ввода.\n')
            continue

    if a==b==0:
        print('Введено 3 нуля. x - любое число' if c==0 else 'Некорректные данные: ' + str(FloatToIntNoLoseCheck(c)) + '=0.')
    elif a==0:
        print('x =', FloatToIntNoLoseCheck(-c/b))
    else:
        D=b**2-4*a*c
        if not D:
            print('x =', FloatToIntNoLoseCheck(-b/a/2))
        elif D<0:
            print('нет действительных решений')
        else:
            print('x1 =', FloatToIntNoLoseCheck((-b+D**0.5)/a/2))
            print('x2 =', FloatToIntNoLoseCheck((-b-D**0.5)/a/2))

def FloatToIntNoLoseCheck(x):
    return int(x) if not (x-int(x)) else x



if __name__ == "__main__":
    Z_6()
