#include <iostream>
#include <locale>
using namespace std;

void Zadanie_04()
{
    int a, b, c;

    while (true)
    {
        cout << "Введите две переменные типа int: ";
        cin >> a >> b;

        if (cin.fail())
        {
            cout << "Ошибка ввода!\n\n";
            cin.clear();
            cin.ignore(32767, '\n');
            continue;
        }
        else { break; }
    }

    cout << "\nВведены переменные: a = " << a << " , b = " << b << endl << endl;

    cout << "== Обмен c участием третьей переменной ==\n\n";
    c = a;
    a = b;
    b = c;
    cout << "Через 3-тью переменную:  a = " << a << ", b = " << b << endl << endl;

    cout << "== Обмен без участия третьей переменной ==\n\n";
    a += b;
    b = a - b;
    a = a - b;
    cout << "Вариант 1(сложение):  a = " << a << ", b = " << b << endl << endl;

    a ^= b;
    b = a ^ b;
    a ^= b;
    cout << "Вариант 2(XOR):  a = " << a << ", b = " << b << endl << endl;
}


void main()
{
    setlocale(LC_ALL, "Russian");
    Zadanie_04();
}
