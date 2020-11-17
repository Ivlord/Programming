#include <iostream>
#include <locale>
using namespace std;

double bmi(double weight, double heigth) {
    return weight / heigth / heigth;
}

int print_bmi(double bmi) {
    double zones[] = { 18.5, 25.0, 30.0, 99.0 };
    string answers[] = { "Underweight", "Normal", "Overweight", "Normal weight" };

    for (int i = 0; i <= 3; i++) {
        if (bmi < zones[i]) {
            cout << answers[i] << endl;
            break;
        }
    }
    return 0;
}

int Zadanie_21() {
    double weight, heigth;
    cin >> weight >> heigth;
    if (cin.fail()) cout << "   ошибка ввода.\n";
    else print_bmi(bmi(weight, heigth / 100));
    return 0;
}

int main() {
    setlocale(LC_ALL, "Russian");
    Zadanie_21();
}
