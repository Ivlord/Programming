import random
import time

# 3 input types:
# int        int  int bool
# [int,..,int]  bool
# [[int,..,int], ... ,[int,..,int]] bool
def BozoSort(a, b=True, c=None, d=True):

    def in_order(ar, ar_size, d, y_size, x_size=0):
        # Функция проверяет, отсортированли массив. Работает с одно- и дву-мерными массивами
        #   Это "внутренняя" функция. Формат получаемых данных НЕ контролируется.
        # <==
        #   (list) ar   - сам массив
        #   (bool) d    - тип сортировки (как должен был быть отсортирован массив): True-по возрастанию, False-по убыванию
        #   y_size      - размер массива в случае одномерного или y-размер в случае двумерного
        #   x_size      - 0 - в случае одномерного или размер по x (в случае двумерного)
        # ==> True-отсортирован по неубыванию, False-отсортирован по невозрастанию
        for i in range(ar_size-1):
            a,b = (ar[i//y_size][i%x_size], ar[(i+1)//y_size][(i+1)%x_size]) if x_size else (ar[i], ar[i+1])
            if (a>b if d else a<b): return False
        return True

    def Bozo_sort_ar(ar, d, y_size, x_size=0):
        # Функция сортировки одно- и двумерных маccивов методом BozoSort.
        #   Это "внутренняя" функция. Формат получаемых данных НЕ контролируется.
        # <==
        #   (list) ar   - сам массив [int,..,int] или "квадратный" [[int,..,int],...,[int,..,int]]
        #   (bool) d    - тип сортировки (как должен был быть отсортирован массив): True-по возрастанию, False-по убыванию
        #   y_size      - размер массива в случае одномерного или y-размер в случае двумерного
        #   x_size      - 0 - в случае одномерного или размер по x (в случае двумерного)
        # ==>
        #   (list) ar   - отсортированный массив
        ar_size, stupid, swaps = len(ar)*len(ar[0]) if x_size else len(ar), 0, 0
        while not in_order(ar, ar_size, d, y_size, x_size):
            idx1, idx2 = random.sample(range(ar_size), 2)
            swaps=swaps+1
            if x_size:  # двумерный
                x1,y1, x2,y2 = idx1%x_size, idx1//y_size, idx2%x_size, idx2//y_size
                stupid+=(ar[y1][x1]<=ar[y2][x2]) if d else (ar[y1][x1]>=ar[y2][x2])
                ar[y1][x1], ar[y2][x2] = ar[y2][x2], ar[y1][x1]
            else:       # одномерный
                stupid+=(ar[idx1]<=ar[idx2]) if d else (ar[idx1]>=ar[idx2])
                ar[idx1], ar[idx2] = ar[idx2], ar[idx1]
        return swaps, stupid

    # 3 int
    if all(isinstance(i, int) for i in [a,b,c]) and isinstance(d,bool):
        a=[a, b, c]
        swaps, stupid=Bozo_sort_ar(a, d, 3)
        return ((a[0], a[1], a[2]) , [swaps, stupid])
    # двумерный массив
    elif isinstance(a, list) and a and isinstance(a[0],list) and isinstance(b, bool) and all(len(a)==len(i) for i in a) and all(isinstance(x, int) for y in a for x in y):
        swaps, stupid=Bozo_sort_ar(a, b, len(a), len(a[0]))
        return (a, [swaps, stupid])
    # одномерный массив
    elif isinstance(a, list) and a and isinstance(b, bool) and all(isinstance(i, int) for i in a):
        swaps, stupid=Bozo_sort_ar(a, b, len(a))
        return (a, [swaps, stupid])
    else: # что-то не так в данных
        print('BozoSort. Формат полученных данных не поддерживается.')
        return False

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

def print_out(dat, d=True):
    t=time.time()
    res=BozoSort(*dat) if d else BozoSort(*dat, False)
    t=time.time()-t
    t=int(t*10)/10
    # можно преределать на вывод без скобок
    #[print(x,' ', sep='', end='') for y in res[0]]
    #[print(x,' ', sep='', end='') for y in res[0] for x in y]
    print(res[0])
    stupid_procent= res[1][1]*100/res[1][0] if res[1][0] else 0
    stupid_procent=int(stupid_procent*10)/10
    print('\t\t\t\t\t', res[1][1],'(', stupid_procent,'%)/', res[1][0], '\t\t', t,sep='')


n=SecureInput([[int, [4,'*']]], hlp='Введите количество элементов (не менее 4-х): \n')[0]
a0=SecureInput([[int]]*n, hlp='Введите сами элементы (целые числа) через пробел: \n')

a1=a0[:]
a2_size=int(n**0.5)
if a2_size*a2_size<n:
    print('Для двумерного массива проигнорируются последних чисел: ', n-a2_size*a2_size, '\n')
a2=[a1[i*a2_size:(i+1)*a2_size] for i in range(a2_size)]

print('\t\t\t\t\tStupid swaps/Swaps\t\ttime,sec')
print_out([a1])
print_out([a1], False)
print_out([a2])
print_out([a2], False)
print_out([a0[:3]])
print_out([a0[:3]], False)
