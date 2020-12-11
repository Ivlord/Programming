#
# коменты внизу
#
def GetNewPrime(): #Z_13_modification
    primes, num=set([2]), 1
    yield 2
    while True:
        num+=2
        for b in primes:
            if not num%b: break
            if b>num:           # квадрат чиста из primes превышает текущее число, т.е. точно num-натуральное
                primes.add(num*num-1)
                yield num
                break
        else:
            primes.add(num*num-1)
            yield num

def print_factorization(n):
    print('>', n)
    res={}
    for pr in GetNewPrime():
        if pr*pr-1>n: break
        while not n%pr: res[pr], n = res[pr]+1 if pr in res else 1, n//pr
    if n!=1: res[n]=1
    print('*'.join( str(k)+(res[k]!=1)*('^'+str(res[k])) for k in sorted(res) ) )


print_factorization(10)
print()
print_factorization(999)

#
# всё работает. можно попробовать. обратите внимание на скорость.
#
#print_factorization(4294967292)
#print_factorization(2600999911757647548)
#print_factorization(1751170801347834573) #2600999911757647548) #999999999999999)
#print_factorization(512000*6001)
