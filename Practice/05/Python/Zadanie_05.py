
def Zadanie_05():
    while True:
        try:
            x0, v0, t = map(float, input('Введите X0, V0 и t через пробел: ').split())
            break
        except:
            print('Ошибка ввода.\n')
            continue

    g=9.8
    print( abs( (x0 + v0 * t - g * t * t * 0.5) - x0) )


Zadanie_05()
