def Z_15():
    import random as rnd
    rnd.seed()
    tries, total_games, total_wins = 5, 1, 0

    while True:
        print('\n\n=================')
        print(    '= Игра угадайка =')
        print(    '=================\n')
        print('Правила игры очень простые:\nмы загадываем число от 0 до 100, а Вы должны отгадать его не более, чем за 5 попыток!')
        print('Поехали!\n\n')
        chislo = rnd.randint(0, 100)
        print('>>',chislo)

        for i in range(tries):
            n=SecureInput([ [int,[0,100]] ], hlp='Попытка №' + str(i+1) + '. Введите число: ', clr='')[0]
            if n==chislo:
                print('Поздравляю! Вы угадали\n')
                total_wins+=1
                break
            else:
                if i==tries-1:
                    print('Вы проиграли. Было загадано:', chislo, '\n')
                else:
                    print('Загаданное число', 'больше\n' if n<chislo else 'меньше\n')

        n=SecureInput([[int]], hlp='Хотите начать сначала? (1 - ДА ) :', clr='')[0]
        if n!=1: break
        total_games+=1
    print('\n\nСтатистика игр')
    print('\n==============')
    print('Игр сыграно     :', total_games)
    print('Игр выиграно    :', total_wins)
    print('Результативность: ', int(1000*total_wins/total_games)/10, '%', sep='' )
    print('\nПриходите ещё! Поиграем на деньги!')


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
    Z_15()
