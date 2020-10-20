#include <iostream>
#include <locale>
using namespace std;

void Zadanie_05()
{
    int t;
    float g = 9.8;
    double x0, v0;

    while (true)
    {
        cout << "Введите X0, V0 и t через пробел: ";
        cin >> x0 >> v0 >> t;

        if (cin.fail())
        {
            cout << "Ошибка ввода.\n\n";
            cin.clear();
            cin.ignore(32767, '\n');
            continue;
        }
        else { break; }
    }

    cout << "\nВведены параметры: X0 = " << x0 << " , V0 =" << v0 << " , t =" << t << "\n\n";

    cout << abs((x0 + v0 * t - g * t * t * 0.5) - x0) << "\n";

}


void main()
{
    setlocale(LC_ALL, "Russian");
    Zadanie_05();
}
