#include <iostream>
#include <locale>
using namespace std;

int Zadanie_9()
{
	int h1, h2, m1, m2;
	char s;
	cout << "Введите время для обоих людей в отдельной строке. Формат ввода: '00:00'.\n";

	while (true) {
		cin >> h1 >> s >> m1;
		if (cin.fail() || !(0 <= h1 <= 23) || !(0 <= m1 <= 59)) {
			cout << "   Первое время введено ошибочно. повторите ввод первого времени.\n";
			cin.clear(); cin.ignore(32767, '\n');
			continue;
		}
		break;
	}

	while (true) {
		cin >> h2 >> s >> m2;
		if (cin.fail() || !(0 <= h2 <= 23) || !(0 <= m2 <= 59)) {
			cout << "   Второе время введено ошибочно. повторите ввод второго времени.\n";
			cin.clear(); cin.ignore(32767, '\n');
			continue;
		}
		break;
	}

	cout << "Встреча" << ((abs(h1 * 60 + m1 - h2 * 60 - m2) <= 15) ? " " : " не ") << "состоится\n";
	return 0;
}

int main()
{
	setlocale(LC_ALL, "Russian");
	Zadanie_9();
	return 0;
}
