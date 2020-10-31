
def Z_21():
    print('================')
    print('= ИМТ проаерка =')
    print('================\n')

    weight, heigth = SecureInput([[float, [0,500]], [float, [0,300]]], hlp='Введите вес и рост (в см): ')
    print_bmi(bmi(weight, heigth/100))

# weight - вес в килограммах
# heigth- рост в метрах.
# Возвращает индекс массы тела.
def bmi(weight, heigth):
    return weight / heigth**2

# Принимает численное значение ИМТ и печатает на экран соответствующую категорию
def print_bmi(bmi):
    print ([i[0] for i in
    [['Underweight',    [ 0.0, 18.5]],
     ['Normal',         [18.5, 25.0]],
     ['Overweight',     [25.0, 30.0]],
     ['Normal weight',  [30.0, 99.9]]
     ] if i[1][0]<=bmi<i[1][1]][0])

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
    Z_21()
