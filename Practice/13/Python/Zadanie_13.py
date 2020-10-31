
def Z_13():
    print('\n\n==============================')
    print(    '= Проверка числа на простоту =')
    print(    '==============================\n')
    n=SecureInput([[int]], separ=' ', hlp='Введите целое число: ', clr='')[0]

    #n=2600999911
    #n=2735691287356145725822111111351

    if n>2 and not n%2:     print('Составное. Делитель = 2')
    elif n>5 and not n%5:   print('Составное. Делитель = 5')
    else:
        flag, primes=False, []

        for a in range(3, int(n**0.5)+1, 2): #if not a%5: continue
            for b in primes:
                if b*b-1>a:
                    primes.append(a)
                    flag=not n%a
                    break
                if not a%b: break
            else:
                primes.append(a)
                flag=not n%a
            if flag: break

        #print('              ',primes)
        print('Составное. Делитель = ' + str(primes[-1]) if flag else 'Простое.')

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
    Z_13()
