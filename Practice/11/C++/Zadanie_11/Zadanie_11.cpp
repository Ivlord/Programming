#include <iostream>
#include <locale>
using namespace std;

double StepenChisla(double base, int stepen) {
    return (stepen == 0) ? 1.0 : base * StepenChisla(base, stepen - 1);
}

void Zadanie_11()
{
    setlocale(LC_ALL, "Russian");
    cout << "===================\n";
    cout << "=  Степень числа  =\n";
    cout << "===================\n\n";

    double base;
    int stepen;

    while (true) {
        cin >> base >> stepen;
        if (cin.fail()) {
            cout << "   ошибка ввода.\n"; cin.clear(); cin.ignore(32767, '\n');
            continue;
        }
        break;
    }
    cout << StepenChisla(base, stepen);

}

int main() {
    Zadanie_11();
    return 0;
}