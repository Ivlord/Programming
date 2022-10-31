import random
# Побитный перевод полученного двоичного числа в массив (бинарную матрицу)
def bin2arr(i, k): #  11 3 => [0,1,1]
    # Получает число i и возвращает list длиной k-элементов, где каждый элемент
    # 0\1 в зависимости от бита в числе
    i = bin(i)[2:]
    return [int(j) for j in ("0"*(k-len(i)) + i) ]                    # fastest
    #return [int(j) for j in (i[:k] if L>=k else (("0"*(k-L)) + i)) ] # fast (проверка)
    #return [1 if (i&(2**b)) else 0  for b in range(k-1,-1,-1)]       # less slow
    #return [int(bool(i&(2**b))) for b in range(k)][::-1]             # slow

def prn_arr(arr):
    #print("[", end="")
    [print(i, sep="", end="") for i in arr]
    #print("]")
# Формирование единичной (квадратной) матрицы заданного размера
GetEd  = lambda k: [[1 if i==j else 0 for i in range(k)] for j in range(k)]

# Транспонирование матрицы
Transp = lambda ar: [[ar[i][j]for i in range(len(ar))] for j in range(len(ar[0]))]

def NKpodbor(k, t = 1): # t = количество ошибок
    n = k + 1
    while (2**k) > ( (2**n) / (n+1) ): n += 1
    p, Dmin = n-k, 2 * t + 1
    print("Биты:: Информационные k=", k, ", проверочные p=", p, ", всего n=", \
    n, ". Dmin=", Dmin, sep="")
    # Формирование массива возможных вариантов проверочных кодов для дополнительной
    # матрицы ,сортируем по убыванию количества единиц в коде
    Hp = sorted( [bin2arr(j, p) for j in range(2**p)], \
                key = lambda stroka: sum(stroka), reverse=True)
    # Оставляем только те коды, сумма единиц в которых (вместе с единецей
    # в единичной матрице) >= Dmin. Обрезаем лишние коды в конце (до
    # размера k). Транспонируем дополнительную матрицу
    H1 = Transp([S for S in Hp if sum(S) >= Dmin - 1][:k])
    # Создаем единичную матрицу размером p (количество проверочных бит)
    Ed_p = GetEd(p)
    H = [H1[i] + Ed_p[i] for i in range(p)] # передается на приемник для декодирования
    return H, H1
# Находим значение бита синдрома ошибки
def Get_Sindrome_Code(k_dat, chk_line): #
    return sum(k_dat[i]*chk_line[i] for i in range(min(len(k_dat), len(chk_line))))&1

def Random_k_bits(k): # ==> int
    return random.randint(0, (2**k) - 1)

# Генерирует
def Transmitter(dat, k):
    info_block = bin2arr(dat, k)
    print("-"*70, "\nПередатчик. ", sep="", end="")
    print("Передается число:\n", dat, " (", "".join(map(str, info_block)), ")", sep="")
    H, H1 = NKpodbor(k, t = 1)
    print("Проверочная матрица, H:")
    [[prn_arr(i), print("")] for i in H]
    add_block = [Get_Sindrome_Code(info_block, i) for i in H1]
    print("Передаваемая информация:")
    prn_arr(info_block)
    print(" ", end="")
    prn_arr(add_block)
    pass_block = info_block + add_block
    # Информ блок + проверочный код, проверочная матрица для
    # приемника (Receiver)
    return pass_block, H

# Линия передач с помехами
#   pass_block - передаваемые данные (информ биты + проверочные - list 0\1)
#   possible_N_of_err_bits - количество бит, в которых может произойти ошибка
#   Er_prob - вероятность наступления ошибки в выбранном
#
def Line_With_Errors(pass_block, Er_prob = 0.2, possible_N_of_err_bits = 1):
    # длина блока, и индексы битов с ошибками
    block_len, error_bit_list = len(pass_block), []
    # коррекция, если вероятность ошибки > 100%
    if Er_prob > 1.0: Er_prob = 1.0
    print("\n", "-"*70, "\nГенерации ошибки в линии с помехами (", Er_prob*100,\
    "%). Получено для передачи:", sep="")
    prn_arr(pass_block)
    # коррекция, если ошибочных бит больше, чем передаваемых
    if possible_N_of_err_bits > block_len: possible_N_of_err_bits = block_len
    # Формируем список индексов битов, где может по вероятности Er_prob произойти
    # ошибка (т.е. бит инвертируется)
    err_bits_idxs = random.sample(list(range(block_len)), possible_N_of_err_bits)
    # Проходим по списку выбранных для ошибки бит, применяем вероятность,
    # инвертируем если надо, добавляем в итоговый список ошибочно переданных бит
    for i in err_bits_idxs:
        if random.uniform(0.0, 1.0) <= Er_prob:
            pass_block[i] = 0 if pass_block[i] else 1
            error_bit_list.append(i)
    print("\nИндексы ошибочно переданных бит: ", error_bit_list)
    print("Измененные данные:")
    prn_arr(pass_block)
    return sorted(error_bit_list)   # индексы бит, в которых была допущена ошибка

def Reciever(pass_block, H):
    print("\n", "-"*70, "\nПриёмник. Получено из линии передачи:", sep="")
    prn_arr(pass_block)

    S = [Get_Sindrome_Code(pass_block, h) for h in H]
    if not sum(S):
        print("\nПередача прошла без ошибок. Код не изменен.")
    else:
        TH=Transp(H)
        err_idx = -1
        for idx in range(len(TH)):
            if TH[idx]==S:
                err_idx=idx
                print("\nПроизошла ошибка в бите с индексом:", err_idx)
                pass_block[err_idx]= 0 if pass_block[err_idx] else 1
                print("Исправленное значение:")
                prn_arr(pass_block)
                break

k = input("Введите количество информационных бит: ")
try:    k = int(k)
except: k = 4
dat = Random_k_bits(k)
pass_block, H = Transmitter(dat, k)
Line_With_Errors(pass_block, 1.0)
Reciever(pass_block, H)
print("\n\nЛабораторная работа №4. Подготовил: Мазлов Иван, ИВТ-201\n")
