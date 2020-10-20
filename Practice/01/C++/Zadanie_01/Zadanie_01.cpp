#include <iostream>
#include <locale>
using namespace std;

void Zadanie_01()
{
    cout << "Результат выражения 2+2*2 = " << 2 + 2 * 2 << std::endl;
}

void main()
{
    setlocale(LC_ALL, "Russian");
    Zadanie_01();
}