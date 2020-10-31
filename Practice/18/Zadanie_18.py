
def Z_18():
    from functools import reduce
    print('\n\n===========')
    print(    '= Евротур =')
    print(    '===========\n')

    words=['hallo', 'klempner', 'das', 'ist', 'fantastisch', 'fluggegecheimen']

    stop=input('> ')
    alph='abcdefghijklmnopqrstuvwxyz'

    words=''.join(words)

    brak1=[i for i in stop if i not in alph] # повводили фигню не по условию задачи
    if brak1:
        print('Во введенном слове есть не строчные латинские символы!')
        print(' '.join(set(brak1)), '\n')

    brak2=[i for i in words if i not in alph]
    if brak2:
        print('"Известные слова" неудачно поправлены при проверке!')
        print('Это НЕ строчные латинские символы:',' '.join(set(brak2)), '\n' )

    brak3=[i for i in stop if i not in words]
    if brak3:
        print('Ну и как подбирать буквы в стоп-слове, если некоторых нет ни в одном "известном слове"?')
        print('Не найденные буквы:', ' '.join(brak3), '\n')

    if (not brak1) and (not brak2) and (not brak3):
        letters={}
        for i in words:
            if i in letters: letters[i]+=1
            else: letters[i]=1

        letters={i:[ letters[i], letters[i] / len(words) ] for i in letters}
        print(reduce((lambda x, y: x * y), [letters[i][1] for i in stop], 1))

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
    Z_18()
