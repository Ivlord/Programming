
def Z_8a(): # Калькулятор с использованием eval (считает намного больше опреаций, чем +-*/)
    print('\n\n=======================================================================')
    print('= Калькулятор с использованием eval (считает намного больше операций) =')
    print('=======================================================================\n')
    while True:
        try:
            print(eval(input('Введите выражение: '))) # ValueError
            break
        except SyntaxError:
            print('   ошибка ввода. Можно вводить только вещественное число и знаки операций.\n')
        except ZeroDivisionError:
            print('   ошибка деление на ноль.\n')
        except Exception:
            print('   ошибка ввода.\n')


def Z_8b(): # Калькулятор обычный
    sup_ops=['+', '-', '*', '/']
    print('\n\n==============================')
    print('= Калькулятор обычный разбор =')
    print('==============================\n')
    while True:
        dat=input('> ').split() # '6 * 7' -> ['6', '*', '7']   '+-*/'
        if dat and len(dat)==1 and dat[0] in ['Quit','quit','Exit','exit','Выход','выход']:
            print('Выход из программы.\n')
            break

        if len(dat)!=3:
            print('   ошибка ввода.\n')
            continue
        if dat[1] not in sup_ops:
            print('   операция не поддерживается.\n')
            continue

        # Варианты ввода:
        # '7*8'           1+= ['7*8']
        # '7* 8'=2        2+=['7*', '8']
        # '7* 8 + 9'=4    4+=['7*', '8', '+', '9']
        # '8 ** 7'=3      3+=['8', '**', '7']
        # '7* 8+ 9'=3     3+=['7*', '8+', '9']
        # 'her48 * idiot' 3=['her48', '*', 'idiot']
        # dat = ['???', '*', '???']

        try:
            a, b, s=float(dat[0]), float(dat[2]), dat[1]
            break
        except:
            print('   ошибка ввода\n')
            continue

    if   s=='+':           res=a+b
    elif s=='-':           res=a-b
    elif s=='*':           res=a*b
    elif s=='/':
        res=a/b if b else 'Делить на ноль нельзя\n'

    print(res)



def Z_8c(): # Калькулятор с использованием import operator
    import operator as op

    print('\n\n=================================')
    print('= Калькулятор c import operator =')
    print('=================================\n')

    while True:
        dat=input('> ').split()
        if dat and len(dat)==1 and dat[0] in ['Quit','quit','Exit','exit','Выход','выход']:
            print('Выход из программы.\n')
            break
        try:
            assert (len(dat)==3)
            print([op.add, op.sub, op.mul, op.truediv]['+-*/'.index(dat[1])](float(dat[0]), float(dat[2]))  )
            break
        except ZeroDivisionError:
            print('   делить на ноль нельзя\n')
        except AssertionError:
            if not dat:
                print('   Не нужно просто нажимать ввод. Формат ввода: <float1><Space><Operator [+-*/]><Space><float2>\n')
                continue
            print('   неправильная структура ввода: ', '   по ТЗ нужно разделять ввод пробелами.\n' if len(dat)<3 else 'слишком много данных.\n')
        except ValueError as ve:
            ve=str(ve)
            if   'substring not found' in ve:
                print('   операция не поддерживается:', dat[1])
            elif 'could not convert string to float' in ve:
                print( '   первое' if ve.split("'")[1]==dat[0] else '   второе', 'число ошибочное:', ve.split("'")[1])
            #print('                      ve:',ve)


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
    Z_8a()
    Z_8b()
    Z_8c()
