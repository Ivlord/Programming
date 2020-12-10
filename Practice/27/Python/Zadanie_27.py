#
#
#
# Я обычно ввожу все данные через свою библиотеку безопасного "с++ похожего" ввода.
# Но в этот раз _дикое_ желание решить задачу в две строки победило )))
#
# 1) могу переделать на "безопасный" ввод.
# 2) если разрешите вводить числа массива через Enter, то получается динамический вариант -
#    хоть на миллиард входящих чисел. Это очень похоже на задачу-27 по динамическому
#    программированию из ЕГЭ:  Zadanie_27_dynamic()


def Zadanie_27():
    n, a=int(input()), list(map(int, input().split())) # а нужен ли тут n в пайтоне при строчном вводе??
    [print(' '.join(  map(str, sorted( a[:i], reverse=True)[-5:]) ) )  for i in range(1, len(a)+1)]


def Zadanie_27_dynamic():
    n=int(input())
    a=[]
    for i in range(n):
        a=sorted(a+[int(input())], reverse=True)[-5:]
        print(' '.join( map(str, a))) # Вывод или тут, после прихода каждого нового числа
    #print(' '.join( map(str, a)))    # или тут по окончании ввода всей последовательности

#Zadanie_27()
Zadanie_27_dynamic()


# Мои библиотеки ввода на манер с++ или почти
##################################################################################
#
def FloatToIntNoLoseCheck(x):
    return int(x) if not (x-int(x)) else x

#       InputType1   Values range   InputType1   NO range, если любые float
# a:[ [ int,        [1, 2]],        [float]             ]
def SecureInput(a, separ=' ', hlp='', clr='(),', different=True): # ==> [2,4.6,'fff',5]
    if not isinstance(a, list) or not a: return []
    while True:
        raw = input(hlp)
        for i in clr: raw = raw.replace(i, ' ')
        raw = raw.split() if separ==' ' else raw.split(separ)
        try:
            if different and len(raw) > len(a): raise Exception('   введены лишние данные\n')
            if different and len(raw) < len(a): raise Exception('   не все данные введены(1)\n')

            res=[ a[i*different][0](raw[i]) for i in range(len(raw))
                if len(a[i*different])==1 or
                ( (-float('inf') if isinstance(a[i*different][1][0], str) else a[i*different][1][0])
                    <= a[i*different][0](raw[i]) <=
                (   float('inf') if isinstance(a[i*different][1][1], str) else a[i*different][1][1])  )]

            if different and len(res)<len(a): raise Exception('   ошибка диапазона\n')
            break

        except LookupError:         print('   не все данные введены(2)\n')
        except ValueError:          print('   ошибка типа данных\n')
        except Exception as err:    print(err)
    return res
