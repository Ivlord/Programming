# ТИК. Лабораторная работа №2. Вариант 4 (но можно любой вариант выполнить)
# Выполнил: ИВТ
import random, math
def form_P(j=None, prob=(0.7, 1.0)): # random.uniform(0.7, 1.0)
    p = [random.randint(1, 10000) for i in R]
    if j!=None:
        p[j]= random.uniform(*prob)
        k= (1.0-p[j]) / (sum(p)-p[j])
    else:
        k= 1.0/sum(p)
    p = [ p[i] if i==j else p[i]*k for i in R]
    p[-1] += 1.0 - sum(p)
    p[-1] += 1.0 - sum(p) # python bug fix
    return p

def main():
    global R
    print("\n" + "-"*70 + "\nТИК. Лабораторная работа №2 (вар.4). Выполнил: xxxxx, ИВТ\n" + "-"*70, "\n")
    N = int(input('Введите количество элементов: '))
    R = tuple(range(N))
    PX = form_P()                                                                   #P(X)
    print("\nМассив вероятностей появления совокупности дискретных сообщений" + \
            " на входе информационного устройства (P(X)):\n", PX, "sum=", sum(PX))
    HX = - sum(el*math.log2(el) for el in PX)                                       #H(X)
    print("\nЭнтропия на входе информационного устройства, на которое поступает" + \
    "совокупность дискретных сообщений {Xi} с вероятностями {p(Xi)}\n","H(X):", HX)
    PXY = [form_P(i, (0.7, 1.0)) for i in R]                                        #(P(X/Y))
    print("\nМассив(2d) вероятностей перехода совокупности входных дискретных сообщений {Xi}" + \
    "в совокупность выходных дискретных сообщений {Yj} ( P(X/Y) ):")
    [print(row, "sum=", sum(row)) for row in PXY]
    PY = [ sum(PX[i]*PXY[j][i] for i in R) for j in R]                              #P(Y)
    print("\nМассив(1d) вероятностей появления совокупности дискретных сообщений" + \
    " на выходе информационного устройства (P(Y)):\n", PY)
    PX_Y = [ [PY[j]*PXY[j][i] for i in R] for j in R]                               #P(X,Y)
    print("\nМассив(2d) совместных вероятностей P(X,Y):")
    [print(row) for row in PX_Y]
    HXY= -sum(PX_Y[i][j]*math.log2(PXY[i][j]) for j in R for i in R)                #H(X/Y)
    print("\nУсловная энтропия (H(Х/Y)), характеризующая остаточную неопределенность" + \
    " принятого сообщения относительной переданного:\n", HXY)
    IX_Y = HX - HXY
    print("\nСреднее количество информации, получаемое" + \
    "при неполной достоверности сообщений I(X,Y):\n", IX_Y,"\n") #I(X,Y)
if __name__=="__main__":
    main()
