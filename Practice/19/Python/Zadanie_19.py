
def Z_19():
    import itertools
    from functools import reduce
    print('===================')
    print('= Медвежья память =')
    print('===================\n')
    n=SecureInput([[int, [1,8]]], hlp='>', clr='')[0] # символов в пароле
    s=input()

    if len(s)==n:
        items = list(itertools.permutations(s, n))
        print(' '.join(p(items)))
    else:
        print('Извините, поддерживается только вариант, когда длина пароля совпадает')
        print('с количеством символов. (в разработке)')
'''
    print()
    items = list(itertools.combinations_with_replacement(s, n))
    print(p(items))

    print()
    items = list(itertools.combinations(s, n))
    print(p(items))

    print()
    items = list(itertools.product('ABCD', 'xy'))
    print(p(items))

    print()
    items = list(itertools.product('ab', 'ab', 'ab'))
    print(p(items))

    print()
    items = list(itertools.product('ab', 'ab', 'ab', 'ab'))
    print(p(items))

    print()
    ss=[s for i in range(n)]
    print(ss)
    items = list(itertools.product( *ss ))
    print(p(items))
'''


def p(a):
    b=[]
    for p in a:
        b.append(''.join(str(x) for x in p))
        #print(''.join(str(x) for x in p))
    return b

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
    Z_19()
