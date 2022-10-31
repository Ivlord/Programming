#
#Вариант 4
#

import math, random

def Poletayev_Zadaniye_1(N, p):
    I=0
    for i in range(0, N):
        I+=(p[i]*math.log2(1/p[i]))
    H=math.log2(N)
    print('среднее количество информации:',I)
    print('максимальная энтропия:',H)

N = int(input('количество элементов:'))
p=[random.randint(1, 10000) for i in range (0, N)]
p_sum=sum(p)
p=[el/p_sum for el in p]
p_sum=sum(p)
if p_sum!=1:
    p[-1] += 1-p_sum

print('--------')
for i in p:
    print(i)
print("Сумма=",sum(p))

Poletayev_Zadaniye_1(N, p)





#print(math.log2(0.0625))
