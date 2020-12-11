#include <iostream>
#include <cstdlib>
#include <vector>
#include <locale>
using namespace std;
typedef struct res_format { unsigned int key; unsigned int value; };

void GetNewPrime(unsigned int& pr, vector<unsigned int>& primes) {
    bool Have_New_Prime;
    if (!pr) { pr = 2; primes.clear(); primes.push_back(2); return; }
    if (pr == 2) pr--;
    while (true) {
        pr += 2;
        Have_New_Prime = true;
        for (auto b : primes) {
            if (!(pr % b)) { Have_New_Prime = false;  break; }
            if (b > pr) break;
        }
        if (Have_New_Prime) {
            primes.push_back(pr * pr - 1);
            return;
        }
    }
}

unsigned int vocabulary_change(vector<res_format>& res, unsigned int key, unsigned int value_change) {
    for (int i = 0; i < res.size(); i++) {
        if (res[i].key == key) {                                        // key found
            if (value_change == 0)               return res[i].value;   // return value
            else { res[i].value += value_change; return res[i].value; }  // Add-return option
        }
    }
    struct res_format tmp = { key, value_change }; // no key. add-init option.
    res.push_back(tmp);
    return value_change;
}

void print_factorization(unsigned int n) {
    bool First_run = true;
    unsigned int pr = 0;
    vector<unsigned int> primes;

    vector<res_format> res;

    GetNewPrime(pr, primes);

    while (pr * pr - 1 <= n) {
        while (!(n % pr)) {
            n = n / pr;
            vocabulary_change(res, pr, 1);
        }
        GetNewPrime(pr, primes);
    }
    vocabulary_change(res, n, 1);

    for (const auto& elem : res)
    {
        if (elem.key == 1) continue;
        if (First_run)  First_run = false; // bez "*" v nachale
        else            cout << "*";

        if (elem.value == 1) cout << elem.key;
        else cout << elem.key << "^" << elem.value;
    }
}

int main()
{
    setlocale(LC_ALL, "Russian");
    unsigned int a; cin >> a;   // unsigned int a= 3072512000; 
    print_factorization(a);
    return 0;    //system("pause");
}
