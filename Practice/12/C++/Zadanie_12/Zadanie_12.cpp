#include <iostream>
#include <locale>
using namespace std;

double Factorial(int n) {
    return (n < 3) ? ((n < 1) ? 1 : n) : n * Factorial(n - 1);
}



void Zadanie_12()
{
    setlocale(LC_ALL, "Russian");
    cout << "=====================\n";
    cout << "=  Факториал числа  =\n";
    cout << "=====================\n\n";

    int n;

    while (true) {
        cin >> n;
        if (cin.fail()) {
            cout << "   ошибка ввода.\n"; cin.clear(); cin.ignore(32767, '\n');
            continue;
        }
        break;
    }
    cout << Factorial(n);

}

int main() {
    Zadanie_12();
    return 0;
}
