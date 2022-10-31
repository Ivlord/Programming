#!/usr/bin/python
import random

poli = [[], #8 7 6 5 4 3 2 1 0     p    # Неприводимые полиномы степени p
            [              1,1], # 1    x + 1
            [            1,1,1], # 2    x^2 + x + 1
            [          1,0,1,1], # 3    x^3 + x + 1
            [        1,0,0,1,1], # 4    x^4 + x + 1
            [      1,0,0,1,0,1], # 5    x^5 + x^2 + 1
            [    1,0,0,0,0,1,1], # 6    x^6 + x + 1
            [  1,0,0,0,0,0,1,1], # 7    x^7 + x + 1
            [1,0,0,0,1,1,0,1,1]] # 8    x^8 + x^4 + x^3 + x + 1


def poliToStr(p):           # Отображение бинарного вида полинома в str формате
    pL = len(poli[p])
    return " + ".join(reversed(
    [( ("1" if i == pL-1 else "x") if i >= pL-2 else ("x^" + str(pL - 1 -i)) )
    for i in range(pL-1, -1, -1)   if poli[p][i] ]))

# Формирование проверочной матрицы соответствия синдромов ошибок для разных бит
# для длины сообщения n и неприводимого полинома степени p (вычисляется по n)
getCheckSindromes = lambda n: [div2( [0]*i+[1]+[0]*(n-i-1) ) for i in range(n)]

# Убирает в полиноме незначащие порядки (нули в бинарном виде) слева
cut = lambda a: a[a.index(1):] if 1 in a else []

# Возвращает параметры сообщения [k, n, p] по одному из параметров k или n
def getKN(x, xIsN=True):
    #   - xIsN=False, то передали k=>   k=x, идет подбор n от x+1, шаг 1
    #   - xIsN=True,  то передали n=>   n=x, идет подбор k от x-1, шаг -1
    x = ([x - 1, x, -1] if xIsN else [x, x + 1, 1]) + [int(not xIsN)]
    while 2**x[0] > 2**x[1] / ( x[1] + 1 ): x[x[3]] += x[2]
    return x[:2] + [ x[1] - x[0] ] # =>[k, n, p]

# Деление полинома полного (n) сообщения на неприводимый полином степени p
def div2(a):                    # возвращает синдром ошибки
    p = getKN(len(a))[2]        # порядок полинома p вычисляется по его длине
    pL, b = len(poli[p]), cut(a[:]) # длина неприводимого полинома, убираем нули
    while len(b) >= pL:
        b = cut([ int(b[i] != poli[p][i]) for i in range(pL) ] + b[pL:])
    return b


dump_arr = lambda arr : "".join(list(map(str, arr))) # вывести массив
pointers = lambda arr: "".join("^" if i in arr else " " for i in range(1,arr[-1]+1))
gen_arr  = lambda k:    [random.randint(0,1) for i in range(k)] # сгнерировать

def cleanInput(s):
    for char in [",;[]()."]: s = s.replace(char, " ")
    return s

# Индексы бит, в которых была допущена ошибка
def Line_With_Errors(pass_block, Er_prob = 0.2, t = 1):
    # длина блока, и индексы битов с ошибками
    block_len, error_bit_list = len(pass_block), []
    # коррекция, если вероятность ошибки > 100%
    if Er_prob > 1.0 or Er_prob < 0.0: Er_prob = 1.0
    print("\n", "-"*70, "\nГенерация "+ ("1 ошибки" if t==1 else "2 ошибок") +\
      " в линии с помехами (", Er_prob*100,\
      "%). Получено для передачи:\n", dump_arr(pass_block), sep="")
    # коррекция, если ошибочных бит больше, чем передаваемых
    if t > block_len: t = block_len
    # Формируем список индексов битов, где может по вероятности Er_prob произойти
    # ошибка (т.е. бит инвертируется)
    err_bits_idxs = random.sample(list(range(block_len)), t)
    # Проходим по списку выбранных для ошибки бит, применяем вероятность,
    # инвертируем если надо, добавляем в итоговый список ошибочно переданных бит
    for i in err_bits_idxs:
        if random.uniform(0.0, 1.0) <= Er_prob:
            pass_block[i] = 0 if pass_block[i] else 1
            error_bit_list.append(i+1)
    print("\nПозиции (!не индексы!) ошибочно переданных бит: ", error_bit_list)
    print("Измененные данные:\n", dump_arr(pass_block), sep="")
    print(pointers(error_bit_list))
    return sorted(error_bit_list)

def lab6():     # Циклический код
    print('\n\nЛабораторная работа №6 ("ЦИКЛИЧЕСКИЙ КОД"). '+\
        'Подготовил: xxx, ИВТ-xxx\n')
    k = input("Введите количество информационных бит k[2; +∞): ")
    try:
        k = int(cleanInput(k))
        if k<2: raise Exception("Неправильный ввод")
    except:
        print(" Установлено значение по умолчанию для Варианта №4: k=58 бит ")
        k = 58

    k, n, p = getKN(k, xIsN=False)
    Dmin = 2 * 1 + 1 # 2*t+1 t=1
    print("Биты:: Информационные k=", k, ", проверочные p=", p, ", всего n=", \
      n, ". Dmin=", Dmin, sep="")

    arr = gen_arr(k)
    print("\n", "-"*70, "\nПередатчик:\n\nСгенерированное случайным образом"+\
      " информационное сообщение:\n", dump_arr(arr), sep="")

    pCode = div2(arr + [0]*p)
    arr = arr + [0]*(p-len(pCode)) + pCode

    print("Закодированное циклическим кодом (полином: ", poliToStr(p), \
    ") сообщение:\n", dump_arr(arr), "\n", "-"*70, "\nПриемник:", sep="")
    errorBits = Line_With_Errors(arr, Er_prob = 1.0, t = 1)

    errorSindrome = div2(arr)
    if errorSindrome:
        errorBitIndex = getCheckSindromes(n).index(errorSindrome)
        arr[errorBitIndex] = 0 if arr[errorBitIndex] else 1
        print("\nПроизошла ошибка в бите №",errorBitIndex + 1,". Исправленные "\
        + "данные:\n", dump_arr(arr),"\n", pointers([errorBitIndex + 1]),sep="")
    else: print("Передача прошла без ошибок")

    print("Раскодированное информационное сообщение:\n",dump_arr(arr[:k]),sep="")

def main():
    lab6()

if __name__=="__main__":
    main()
