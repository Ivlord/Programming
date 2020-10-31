
def Z_10a():
    print('================================')
    print('= Сиспользованием map и filter =')
    print('================================\n')

    raw=SecureInput([[int], [int], [int], [int], [int] ], separ=' ', hlp='Введите целые числа S, L1, R1, L2, R2: ', clr='')
    s, s1, s2 = raw[0], range(raw[1], raw[2]+1), range(raw[3], raw[4]+1)

    r=list(map( lambda a: ( [a, list(filter(lambda b: b+a==s, s2))]  )  , s1))
    r=list(filter(lambda a: len(a[1])==1, r))

    if r:
        r=sorted(r, key=lambda x: x[0])
        print(r[0][0], r[0][1][0])
    else: print(-1)


def Z_10b():
    print('\n\n=========================================')
    print('= Сиспользованием генератора списков    =')
    print('== внутри генератора циклы есть однако ==\n')

    raw=SecureInput([[int], [int], [int], [int], [int] ], separ=' ', hlp='Введите целые числа S, L1, R1, L2, R2: ', clr='')
    s, s1, s2 = raw[0], range(raw[1], raw[2]+1), range(raw[3], raw[4]+1)

    r=sorted([[x,y] for x in s1 for y in s2 if x+y==s] , key=lambda x: x[0], reverse=False)
    print(str(r[0][0]) + ' ' + str(r[0][1]) if r else -1)



def S2cycle(s2, idx_s2, x1, s, final_array):
    if idx_s2==len(s2):  return final_array
    if s2[idx_s2]+x1==s: final_array.append([x1, s2[idx_s2]])
    return S2cycle(s2, idx_s2+1, x1, s, final_array)

def S1cycle(s1, s2, idx_s1, s, final_array):
    if idx_s1==len(s1): return final_array
    return S1cycle(s1, s2, idx_s1+1, s, final_array + S2cycle(s2, 0, s1[idx_s1], s, [])  )


def Z_10c():
    print('\n\n===========================================')
    print(    '= С использованием рекурсии в виде циклов =')
    print(    '===========================================\n')

    raw=SecureInput([[int], [int], [int], [int], [int] ], separ=' ', hlp='Введите целые числа S, L1, R1, L2, R2: ', clr='')
    s, s1, s2 = raw[0], range(raw[1], raw[2]+1), range(raw[3], raw[4]+1)

    r=sorted(S1cycle(s1, s2, 0, s, []) , key=lambda x: x[0], reverse=False)
    print(str(r[0][0]) + ' ' + str(r[0][1]) if r else -1)



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
    Z_10a()
    Z_10b()
    Z_10c()
