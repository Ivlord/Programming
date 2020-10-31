#include <iostream>
#include <locale>
#include <cmath>
using namespace std;

void Zadanie_06()
{
    setlocale(LC_ALL, "Russian");

    cout << "===================\n";
    cout << "= Корни уравнения =\n";
    cout << "===================\n\n";

    double a, b, c, d;

    while (true)
    {
        cout << "'Введите a,b,c - вещественные числа:'\n";
        cin >> a >> b >> c;
        if (cin.fail())
        {
            cout << "   ошибка ввода.\n";
            cin.clear();
            cin.ignore(32767, '\n');
            continue;
        }
        break;
    }
    if (a == 0 && b == 0) cout << (c == 0) ? "Введено 3 нуля. x - любое число" : "Некорректные данные.";
    else if (a == 0) cout << "x = " << -c / b;
    else {
        d = pow(b, 2) - 4 * a * c;
        if (d == 0) cout << "x = " << -b / a / 2;
        else if (d < 0) cout << "нет действительных решений";
        else {
            cout << "x1 = " << (-b + pow(d, 0.5)) / a / 2 << endl;
            cout << "x2 = " << (-b - pow(d, 0.5)) / a / 2 << endl;
        }


    }
}

int main() {
    Zadanie_06();
    return 0;
}