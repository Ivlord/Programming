def Z_17():
    print('\n\n==========')
    print(    '= Казино =')
    print(    '==========\n')
    games=[]
    black=[2,4,6,8,10,11,13,15,17,20,22,24,26,28,29,31,33,35]
    while True:
        x=SecureInput([[int, [-36,36]]], hlp='> ', clr='')[0]
        if x<0: break
        games.append(x)
        if len(games)>12: games=games[:12]
        stat={} # {1:2, 2:4 ...}
        for i in games:
            if i in stat: stat[i]+=1
            else: stat[i]=1
        stat=sorted([ [stat[i], i] for i in stat])              # [ [количество, номер], ... ]
        stat=sorted([ i[1] for i in stat if i[0]==stat[0][0]])  # оставляем только номера с максимумами
        print( ' '.join(list( map(str,stat)   ) ) )             # вывели максимальные номера
        for i in range(37):
            if i not in games: print(i, ' ', sep='', end='')    # вывели не выпадавшие
        red =sum([1 for i in games if i not in black and i!=0]) # отсекаем ноль, т.к. он зеленый
        blck=sum([1 for i in games if i in black])
        print()
        print(red, blck, '\n')


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
    Z_17()
