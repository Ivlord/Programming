#include <iostream>
#include <locale>
using namespace std;

void Zadanie_02()
{
    int a = 3;        // объявление переменной a целого типа (integer). 
    float b = 3.14;   // объявление переменной b типа данных с плавающей запятой (float).
    double c = 3.0;   // инициализация переменной типа double.
    double d = 3.14;  // инициализация переменной типа double. 

    cout << "int    a = " << a << endl;
    cout << "float  b = " << b << endl;
    cout << "double c = " << c << endl;
    cout << "double d = " << d << endl;
}

void main()
{
    setlocale(LC_ALL, "Russian");
    Zadanie_02();
}
