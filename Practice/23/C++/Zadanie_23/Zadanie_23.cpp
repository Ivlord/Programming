#include <iostream>
#include <iomanip>      // std::setprecision
#include <locale>
#include "factorial.h"
#include "sinx.h"
#include "sochet.h"
using namespace std;


int main()
{
    setlocale(LC_ALL, "Russian");


    std::cout << "n\tn!\n"; // "таблицу факториалов в диапазоне"
    for (int i = 1; i <= 10; i++)
    {
        std::cout << std::setprecision(10) << i << "\t" << factorial(i) << endl;
    }

    double pi = 3.14159;
    std::cout << "\nx\tsin(x)" << endl; // таблицу синуса угла в диапазоне 
    for (double i = 0; i <= pi / 4; i += pi / 180) {
        std::cout << std::setprecision(4) << i << "\t" << sinx(i, 5) << endl;
    }

    std::cout << "\nk\tC(k, 10)" << endl; // таблицу сочетаний для n=10, k in range[1,11]
    for (int k = 1; k <= 10; k++) {
        std::cout << std::setprecision(4) << k << "\t" << sochet(10, k) << endl;
    }

    return 0;
}
