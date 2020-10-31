#include <iostream>
#include <locale>
using namespace std;


void Zadanie_14()
{
    setlocale(LC_ALL, "Russian");
    cout << "====================\n";
    cout << "=  Степень двойки  =\n";
    cout << "====================\n\n";

    int cnt = 0;
    unsigned long long n;

    while (true) {
        cin >> n;
        if (cin.fail()) {
            cout << "   ошибка ввода.\n"; cin.clear(); cin.ignore(32767, '\n');
            continue;
        }
        break;
    }

    while (n)
    {
        cnt += 1;
        n = n >> 1;
    }

    cout << cnt;

}

int main() {
    Zadanie_14();
    return 0;
}