
def Z_12a():
    print('\n\n=======================================')
    print(    '= Факториал с использованием рекурсии =')
    print(    '=======================================\n')

    n=SecureInput([[int]], separ=' ', hlp='Введите целое число: ', clr='')[0]
    print(Factorial(n))

def Factorial(n):
    return (1 if n<1 else n) if n<3 else n*Factorial(n-1)

def Z_12b():
    print('\n\n===========================')
    print(    '= Факториал с while и for =')
    print(    '===========================\n')

    n=SecureInput([[int]], separ=' ', hlp='Введите целое число: ', clr='')[0]

    res=1
    for i in range(2,n+1):
        res*=i
    print('с for: ', res)

    res=1
    while n>1:
        res*=n
        n-=1
    print('с while: ', res)

def Z_12c():
    from functools import reduce
    print('\n\n======================')
    print(    '= Факториал с reduce =')
    print(    '======================\n')

    n=SecureInput([[int]], separ=' ', hlp='Введите целое число: ', clr='')[0]
    print(reduce((lambda a, b: a * b), range(2,n+1)))

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
    Z_12a()
    Z_12b()
    Z_12c()
