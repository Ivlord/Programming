#include <iostream>
#include <locale>
using namespace std;

void Zadanie_03()
{
    int a, b;
    double c, d;

    while (true)
    {
        cout << "Введите две переменные типа int и две переменные типа double: ";
        cin >> a >> b >> c >> d;

        if (cin.fail())
        {
            cout << "Ошибка ввода!\n\n";
            cin.clear();
            cin.ignore(32767, '\n');
            continue;
        }
        else { break; }
    }

    cout << endl << a << "," << b << "," << c << "," << d << endl << endl;

    cout << "int + int = " << a + b << endl;
    cout << "int - int = " << a - b << endl;
    cout << "int * int = " << a * b << endl;
    cout << "int / int = " << a / b << endl << endl;

    cout << "double + double = " << c + d << endl;
    cout << "double - double = " << c - d << endl;
    cout << "double * double = " << c * d << endl;
    cout << "double / double = " << c / d << endl << endl;

    cout << "int + double = " << a + c << endl;
    cout << "int - double = " << a - c << endl;
    cout << "int * double = " << a * c << endl;
    cout << "int / double = " << a / c << endl << endl;

    cout << "double + int = " << c + a << endl;
    cout << "double - int = " << c - a << endl;
    cout << "double * int = " << c * a << endl;
    cout << "double / int = " << c / a << endl << endl;

}


void main()
{
    setlocale(LC_ALL, "Russian");
    Zadanie_03();
}
