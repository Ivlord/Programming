
#
# Задание 20 в процессе доделывания.
#

def EvalPrice(a, top): # [1,2,3]
    return sum([bottle_types[i][1] for i in a])<=top

def NexSet(a, MaxBottlesPossible, bottle_types_len, CurIDX):
    if CurIDX==MaxBottlesPossible: return []
    if CurIDX==len(a):
        a=[0 for i in range(CurIDX+1)]
        return a

    a[CurIDX]+=1
    if a[CurIDX]==bottle_types_len:
        a[CurIDX]=0
        return NexSet(a, MaxBottlesPossible, bottle_types_len, CurIDX+1)
    return a

########################################################################
#def Z_20():

# bottle set:    (bottles_idx)
# bottle_types:  (Name, Price, Liters, LiterPrice)

bottle_types = [('Vodka', 40, 1) , ('Pivo', 30, 2), ('Spirt', 80, 1)]
bottle_types_len=len(bottle_types)
bablo=1000
MaxLiter=0
MaxList=[]

#print('bottle_types:',bottle_types)

MaxBottlesPossible = int(bablo/sorted(bottle_types, key=lambda x: x[1])[0][1]) #получаем самую длинную серию бутылок
MinBottlesPossible = int(bablo/sorted(bottle_types, key=lambda x: x[1], reverse=True)[0][1]) #получаем самую длинную серию бутылок

a=[0 for i in range(MinBottlesPossible)]
while a:
    print('     ',a)
    if EvalPrice(a, bablo):
        L = sum([bottle_types[i][2] for i in a])
        if   L==MaxLiter:
            MaxList.append(a[:])
            print('   MaxList>', MaxList)
        elif L>MaxLiter:
            MaxLiter, MaxList=L, [a[:]]
            print('   L=',L, '  ',MaxList)
    a=NexSet(a, MaxBottlesPossible, bottle_types_len, 0)

print('=============================')
print('MaxLiter=',MaxLiter)
for i in MaxList:
    print('  ',i)


#if __name__ == "__main__":
#    Z_20()
