

def Z_7(): #
    v = SecureInput([ [int, [1,2]] ], hlp='Введите вариант рассчета (1-длины сторон или 2-координаты вершин)> ')
    if v[0]==1:
        a = SecureInput([[float]], hlp=' сторона a> ')[0]
        b = SecureInput([[float]], hlp=' сторона b> ')[0]
        c = SecureInput([[float]], hlp=' сторона c> ')[0]
    else:
        A = SecureInput([[float], [float]], hlp=' вершина A (x,y)> ')
        B = SecureInput([[float], [float]], hlp=' вершина B (x,y)> ')
        C = SecureInput([[float], [float]], hlp=' вершина C (x,y)> ')
        a=((A[0]-B[0])**2 + (A[1]-B[1])**2)**0.5
        b=((A[0]-C[0])**2 + (A[1]-C[1])**2)**0.5
        c=((C[0]-B[0])**2 + (C[1]-B[1])**2)**0.5
    p=(a+b+c)/2
    print('S =', FloatToIntNoLoseCheck((p*(p-a)*(p-b)*(p-c))**0.5 ))
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
    Z_7()
