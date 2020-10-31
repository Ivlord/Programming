#include <iostream>
#include <locale>
#include <cmath>
using namespace std;

void Zadanie_07()
{
    setlocale(LC_ALL, "Russian");
    cout << "========================\n";
    cout << "= Площадь треугольника =\n";
    cout << "========================\n\n";

    double a, b, c, p, Ax, Ay, Bx, By, Cx, Cy;
    int vibor;

    while (true) {
        cin >> vibor;
        if (cin.fail()) {
            cout << "   ошибка ввода.\n";
            cin.clear(); cin.ignore(32767, '\n'); continue;
        }

        if (vibor == 1) {
            cin >> a >> b >> c;
            if (cin.fail()) {
                cout << "   ошибка ввода.\n";
                cin.clear(); cin.ignore(32767, '\n'); continue;
            }
        }
        else if (vibor == 2) {
            cin >> Ax >> Ay;
            if (cin.fail()) {
                cout << "   ошибка ввода.\n";
                cin.clear(); cin.ignore(32767, '\n'); continue;
            }
            cin >> Bx >> By;
            if (cin.fail()) {
                cout << "   ошибка ввода.\n";
                cin.clear(); cin.ignore(32767, '\n'); continue;
            }
            cin >> Cx >> Cy;
            if (cin.fail()) {
                cout << "   ошибка ввода.\n";
                cin.clear(); cin.ignore(32767, '\n'); continue;
            }

            a = pow(pow(Ax - Bx, 2) + pow(Ay - By, 2), 0.5);
            b = pow(pow(Ax - Cx, 2) + pow(Ay - Cy, 2), 0.5);
            c = pow(pow(Cx - Bx, 2) + pow(Cy - By, 2), 0.5);

        }
        else {
            cout << "   ошибка ввода.\n";
            cin.clear(); cin.ignore(32767, '\n'); continue;
        }
        p = (a + b + c) / 2;
        cout << "S = " << pow((p * (p - a) * (p - b) * (p - c)), 0.5);

        break;
    }

}

int main()
{
    Zadanie_07();
    return 0;
}