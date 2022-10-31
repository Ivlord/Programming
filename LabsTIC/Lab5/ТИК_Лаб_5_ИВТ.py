from functools import reduce as red
from math      import log2   as log2, ceil as ceil
import random

dump_arr = lambda arr : "".join(list(map(str, arr)))
gen_arr  = lambda k:    [random.randint(0,1) for i in range(k)]

def insert_check_bits(arr, n):
    for power in range(ceil(log2(n))):
        arr.insert((2**power)-1, "-")

def del_check_bits(arr):
    for power in range(int(log2(len(arr))), -1, -1):
        del arr[(2**power)-1]

#      x x   x       x
#      0 1 2 3 4 5 6 7 8 9 0 1 2 3 4
# a = [8,8,1,8,0,1,0,8,0,1,1,1,0,1,1]

def Get_Haming_bit(power, arr):
    st, L, res = 2**power, len(arr), []
    for start in range(st-1, L, 2*st):
        res+=arr[start : min(start+st, L)]
    return sum(res[1:])&1

def Haming(arr, Check=True):
    res = 0
    for power in range(int(log2(len(arr)))+1):
        if Check:   # Check bit
            if Get_Haming_bit(power, arr)!=arr[(2**power)-1]:
                res += 2**power
        else:       # Set bit
            HB = Get_Haming_bit(power, arr)
            arr[(2**power)-1] = HB
    return res

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

    # для теста повреждения бита четности
    #pass_block[-1] = 0 if pass_block[-1] else 1
    #error_bit_list.append(len(pass_block))

    print("\nПозиции (!не индексы!) ошибочно переданных бит: ", error_bit_list)
    print("Измененные данные:\n", dump_arr(pass_block), sep="")
    return sorted(error_bit_list)   # индексы бит, в которых была допущена ошибка

def lab5():
    print('\n\nЛабораторная работа №5 ("КОД ХЕММИНГА"). '+\
        'Подготовил: xxx, ИВТ-xx\n')
    k = input("Введите количество информационных бит k[2; +∞), "+\
      "кол-во ошибок t[1; 2]"+\
      " и \n  вероятность ошибки в линии в процентах Err[0; 100] через пробел."+\
      "\n  (Например: 11 2 50): ")
    try:
        k, t, Err = map(int, k.replace(",", " ").replace(";", " ").\
             replace("]", " ").replace("[", " ").replace(")", " ").\
             replace("(", " ").split())
        if t not in [1,2]:
            print(" Установлено значение по умолчанию для количества ошибок. t=1")
            t = 1
        if not 0<=Err<=100:
            Err = 50
            print(" Установлено значение по умолчанию вероятности ошибки. Err=50%")
    except:
        print(" Установлено значение по умолчанию для Варианта №4: 11 бит "+\
        "(единичная ошибка, Err=50%)\n")
        k = 11
        t = 1
        Err = 50

    Err/= 100
    n = k + 1
    while (2**k) > ( (2**n) / (n+1) ): n += 1
    p, Dmin = n-k, 2 * t + 1
    print("Биты:: Информационные k=", k, ", проверочные p=", p, ", всего n=", \
      n, ". Dmin=", Dmin, sep="")

    arr = gen_arr(k)
    print("\n", "-"*70, "\nПередатчик:\n\nСгенерированное случайным образом"+\
      " информационное сообщение:\n", dump_arr(arr), sep="")
    insert_check_bits(arr, n)
    print("Освобождаем позиции для проверочных битов", " и бита четности в "+\
      "конце" if t==2 else "" ,":\n", dump_arr(arr), "x" if t==2 else "", sep="")

    Haming(arr, False)
    print("Закодированное кодом Хемминга сообщение:\n", dump_arr(arr), sep="")

    if t==2: # есть две ошибки, добавляем бит четности в конце кода
        arr+= [sum(arr)&1]
        print("Добавлен бит четности в конце:\n", dump_arr(arr), sep="")

    Line_With_Errors(arr, Err, t)

    #Line_With_Errors(arr, 1.0, 1) # тест варианта с одной ошибкой

    print("\n", "-"*70, "\nПриемник:", sep="")


    # если стояло, что может быть 2 ошибки, то значит есть
    # ParityBit. Генерируем его из сообщения и сравниваем с тем,
    # что получили по линии связи
    if t==2:
        # Получаем результирующий бит чености:
        # полученный (сумма модуля 2) рассчитанный заново по сообщению
        #ParityBit = (arr[-1] + sum(arr[:-1]))&1
        ParityBit = sum(arr)&1
        arr = arr[:-1]  # обрезаем бит четности
    else:
        ParityBit = 0

    res = Haming(arr)

    # Есть 4 возможных варианта:
    # 1) ParityBit=0 и Hamming=0 => ошибок не было
    # 2) ParityBit=0 и Hamming=1 => двойная ошибка!! ничего нельзя сделать
    # 3) ParityBit=1 и Hamming=0 => 1 ошибка и попала на ParityBit. => 1)
    # 4) ParityBit=1 и Hamming=1 => 1 ошибка. Исправляем код по Хаммингу.
    #      0                 0
    if (not res) and not ParityBit:
        print("\nПередача прошла без ошибок. Код не изменен.")
        del_check_bits(arr)
        print(dump_arr(arr))

    # бит четности есть, но код не совпадает = двойная ошибка
    #     1              0
    elif res and not ParityBit and t==2:
        print("\nПри передаче прошла двойная ошибка."+\
        " Декодирование невозможно!")

    #       0            1
    elif (not res) and ParityBit: # поврежден check_bit, информблок нормальный
        print("\nПри передаче была одна ошибка, но повредился именно бит"+\
        " четности.\nПередача информационного сообщения прошла без ошибок."+\
        " Принятые данные:", sep="")
        del_check_bits(arr)
        print(dump_arr(arr))
    #     1            1
    else:
        print("\nПроизошла единичная ошибка в бите на позиции"+\
        " (не индексе!):", res)
        print("Исправляем ошибку:")
        arr[res-1] = 0 if arr[res-1] else 1
        print("Исправленное информационное сообщение:", sep="")
        del_check_bits(arr)
        print(dump_arr(arr))

def main():
    #a = [8,8,1,8,0,1,0,8,0,1,1,1,0,1,1]
    lab5()

if __name__=="__main__":
    main()
