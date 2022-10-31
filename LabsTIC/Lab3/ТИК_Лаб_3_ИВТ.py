# ТИК. Лабораторная работа №2. Вариант 4 (но можно любой вариант выполнить)
import random, math

def header(lab, var):
    global R, N
    print("\n" + "-"*70 + "\nТИК. Лабораторная работа №" + str(lab) + \
    " (вар." + str(var) + "). Выполнил: xxx, ИВТ-xxx\n" + "-"*70, "\n")
    try:
        N = int(input('Введите количество элементов: '))
    except:
        N = 4
    R = tuple(range(N))

def print_2d_table(a, add_sum=False):
    lens=[ max( len(str(a[j][i]))    for j in range(len(a)) )
          for i in range(len(a[0]))]
    for j in range(len(a)):
        print(" [", end="")
        for i in range(len(a[j])):
            s=str(a[j][i])
            s+= "0"*(lens[i]-len(s)) + (", " if i!=len(a[j])-1 else "")
            print(s, end="")
        print("]", " Σ="+ str(sum(a[j])) if add_sum else "")

def range_bits(val, r=50): #val=[1, ~)
    # перевод бит в [бит,Кбит,Мбит,Гбит,Тбит]
    #        бит/c             Кбит                Мбит                Гбит
    #( (1,""), (10**3, " бит/с"), (10**6, " Кбит/с"), (10**9, " Мбит/с"), (10**12, " Гбит/с") )
    #   0           1                 2                3                    4
    L = ["","К","М","Г","Т"]
    return "".join( str( round(val/(10**(3*i-3)), r) )+" "+ L[i-1]+"бит/с"
            if 10**(3*i-3)<=val<10**(3*i) else ""
            for i in range(1, len(L)+1 ) )

class infoSyS():
    def __init__(self, N, prob=(10**(-5), 1.0, False), name="test"):
        self.N          = N             # количество элементов массива (символов)
        self.prob       = prob          # вероятность для безошибочного или ошибочного, True-беошиб./False-ошибочн
        self.name       = name          # имя теста
        self.R          = range(N)      # range для массива, чтоб не вычислять каждый раз

        # Вероятность появления символов на входе P(Y)
        self.PY     = self.form_P()

        # Энтропия информационного сигнала на входе в канал передачи. H(Y) =
        self.HY     = - sum(y * math.log2(y) for y in self.PY)

        # Массив длительности символов τ =
        self.t = [random.uniform(10**(-10), self.N) for i in self.R]

        # Средняя длительность символа τ_
        self.t_ = sum(self.PY[i] * self.t[i] for i in self.R)/10**6

        # Идеальная средняя скорость передачи символов в сек без помех. Uy (1/τ_) =
        self.Uy=1/self.t_

        # Скорость передачи для дискретного канала без помех. I=
        self.I = self.Uy * self.HY

        # Пропускная (макс) способность дискретного канала без помех характеризуется
        #  максимальной энтропией информационного сигнала. C ="
        self.C = self.Uy * math.log2(self.N) #  =math.log2(self.N) / self.t_

        # Матрица(2d) переходов. P(Y/Z)
        self.PYZ = [self.form_P(prob=(0, 0.5/self.N, False), idx=j)
                    for j in self.R]

        # Вероятность появления дискретных сообщений Zj на выходе
        self.PZ = [ sum(self.PY[i] * self.PYZ[j][i] for i in self.R)
                    for j in self.R]

        # Совместная вероятность появления сообщения на входе Yi и на выходе Zj P(Yi,Zj)
        self.PY_Z = [ [self.PZ[j] * self.PYZ[j][i] for i in self.R]
                     for j in self.R]

        # Остаточная энтропия сигнала, обусловленная действием помех. (H(Y/Z))
        self.HYZ = -sum(self.PY_Z[i][j] * math.log2(self.PYZ[i][j])
                        for j in self.R for i in self.R)

        # Скорость передачи информации по дискретному каналу с помехами I(Z,Y)
        self.IZ_Y = (self.HY - self.HYZ) / self.t_

        # Пропуская способность дискретного канала с помехами
        self.C_ = (math.log2(self.N) - self.HYZ) / self.t_

    def prn(self):
        print("")
        if self.name: print("\n", "="*10, " Тест: " + str(self.name), " ", "="*10 )
        print("\nВероятность появления символов на входе P(Y): \n", self.PY," Σ =", sum(self.PY))
        print("\nМассив длительности символов(мкс) τ =", self.t)
        print("Средняя длительность символа τ_ =", self.t_, "с")
        print("\nИдеальная средняя скорость передачи без помех. \t\t\t\t   Uy(1/τ_) =", self.Uy, "симв./сек")
        print("Энтропия информационного сигнала на входе в канал передачи. \t\t   H(Y)\t    =", self.HY, "бит")
        print("Скорость передачи для дискретного канала без помех. \t\t\t   I \t    =", range_bits(self.I))
        print("Пропускная (макс) способность дискретного канала без помех характеризуется\n" + \
              "  максимальной энтропией информационного сигнала. \t\t\t   C \t    =", range_bits(self.C))
        print("\nМатрица(2d) переходов. P(Y/Z):")
        print_2d_table(self.PYZ, True)
        print("\nВероятность появления дискретных сообщений Zj на выходе. P(Z):")
        print(self.PZ)
        print("\nСовместная вероятность появления сообщения на входе Yi и на выходе Zj. P(Y,Z):")
        print_2d_table(self.PY_Z)
        print("\nОстаточная энтропия сигнала, обусловленная действием помех. \tH(Y/Z) =", self.HYZ, "бит")
        print("Скорость передачи информации по дискретному каналу с помехами \tI(Z,Y) =", range_bits(self.IZ_Y))
        print("Пропуская способность дискретного канала с помехами \t\tС_     =", range_bits(self.C_))

    def form_P(self, prob=(10**(-5), 1.0, True), idx=None):
        # формирование 1d массива случайных вероятностей (частный случай с заданной
        # вероятностью безошибочной передачи сообщения или вероятностью ошибочной передачи)
        #   idx - индекс ключевого элемента (если None, то случайно раскидывает всю строку)
        #   [prob[0];prob[1]] - границы распределения вероятности
        #   prob[2] = True = Вероятность безошибочной передачи
        #   prob[2] = False = вероятность ошибки в канале

        p = [random.uniform(*prob[:2]) for i in self.R]
        # строка вероятностей:
        if idx==None:         # случайных (границы весов prob[:2]
                              # Ex: (1,1)- для равновероятных событий)
            k= 1.0/sum(p)
            p = [p[i]*k for i in self.R]
        elif prob[2]:       # с указанной для элемента idx вероятностью
                            # (границы prob[:2]) безошибочной передачи.
                            # ошибочные - случайные
            p[idx]= random.uniform(*prob[:2])
            k= (1.0-p[idx]) / (sum(p)-p[idx])
            p = [ p[i] if i==idx else p[i]*k for i in self.R]
        else:               # idx-элемент с вероятностью безошибочной передачи
                            # и указанным диапазоном вероятностей для ошибки в
                            # канале (границы prob[:2])
            p[idx] = 1.0 - (sum(p) - p[idx])

        while sum(p)!=1.0:
            p[-1] += 1.0 - sum(p) # python bug fix
        return p

def lab3():
    global N
    header(3, 4)
    N_test = 6  # количество тестов
    tests = [infoSyS(N, prob=(10**(-9), 0.5/N), name="Тест "+str(i+1))
             for i in range(N_test)]

    for i in range(N_test):
        tests[i].prn()
        if i!=N_test-1:
            tmp = input("\nнажмите enter для следующего теста\n")

    print("\nСводная таблица тестов:")
    print("-"*95)
    print("Name   t ср,мкс\t H(Y)\t\tI\t    C\t\tH(Y/Z)\t    I(Z,Y)\t     C_\n"+"-"*95)

    for i in range(N_test):
        print(tests[i].name, "\t", round(tests[i].t_*10**6,3), "\t", \
        round(tests[i].HY,4), "\t", range_bits(tests[i].I, 3), "\t", \
        range_bits(tests[i].C, 3), "\t", round(tests[i].HYZ,4), "\t", \
        range_bits(tests[i].IZ_Y,3), "\t", range_bits(tests[i].C_,3), sep="")

def main():
    lab3()

if __name__=="__main__":
    main()


#print("{0} {1:1.5f} {2:1.5f} {3:1.5f} {4:1.5f} {5:1.5f} {6:1.5f} {7:1.5f}".format(tests[i].name, tests[i].t_, tests[i].HY, tests[i].I, tests[i].C, tests[i].HYZ, tests[i].IZ_Y, tests[i].C_))
