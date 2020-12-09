#include <iostream>
#include <locale>
#include <vector>
#include <string>
#include <math.h>
using namespace std;

void Vector_cout(vector<int> vect, string sep = "") {
	for (int x = 0; x < vect.size(); x++) cout << vect[x] << " ";
	cout << sep;
}

void Vector_cout(vector<vector<int>> vect) {
	for (int y = 0; y < vect.size(); y++) Vector_cout(vect[y]);
	cout << endl;
}

void Clear_input(string txt) {
	cout << txt << endl;
	cin.clear();
	cin.ignore(32767, '\n');
}

void Bozo_sort_ar(vector<int>& vect, bool sort_type = true) {
	srand(time(NULL));
	int idx1 = rand() % vect.size(), idx2 = rand() % vect.size(), temporary;
	bool need_sorting = true;

	while (need_sorting) {
		while (idx1 == idx2) {
			idx1 = rand() % vect.size();
			idx2 = rand() % vect.size();
		}
		temporary = vect[idx1]; vect[idx1] = vect[idx2]; vect[idx2] = temporary;

		need_sorting = false;
		for (int i = 1; i < vect.size(); i++) {
			if ((sort_type && (vect[i - 1] > vect[i])) || (!sort_type && (vect[i - 1] < vect[i]))) {
				need_sorting = true; break;
			}
		}
		idx1 = idx2;
	}
}

vector<int> BozoSort(vector<int> vect, bool sort_type = true) {
	Bozo_sort_ar(vect, sort_type);
	return vect;
}

//struct res { int a; int b; int c; };
vector<int> BozoSort(int a, int b, int c, bool sort_type = true) {
	vector<int> ints{ a,b,c };
	Bozo_sort_ar(ints, sort_type);
	return ints;
}

vector<vector<int>> BozoSort(vector<vector<int>> vect, bool sort_type = true) {
	srand(time(NULL));
	int vect_size = vect.size();
	int elements_in_vect = vect_size * vect_size;

	int x1, y1, x2, y2, idx1 = 0, idx2 = 0, temporary;
	bool need_sorting = true;

	while (need_sorting) {
		while (idx1 == idx2) {
			idx1 = rand() % elements_in_vect;
			idx2 = rand() % elements_in_vect;
		}
		y1 = idx1 / vect_size; x1 = idx1 % vect_size;
		y2 = idx2 / vect_size; x2 = idx2 % vect_size;

		temporary = vect[y1][x1]; vect[y1][x1] = vect[y2][x2]; vect[y2][x2] = temporary;

		need_sorting = false;
		for (int i = 1; i < vect.size(); i++) {
			if ((sort_type && (vect[i - 1] > vect[i])) || (!sort_type && (vect[i - 1] < vect[i]))) {
				need_sorting = true; break;
			}
		}

		need_sorting = false;
		temporary = vect[0][0];
		for (int y = 0; y < vect_size; y++) {
			for (int x = 0; x < vect_size; x++) {
				if ((sort_type && (temporary > vect[y][x])) || (!sort_type && (temporary < vect[y][x]))) {
					need_sorting = true; break;
				}
				temporary = vect[y][x];
			}
			if (need_sorting) break;
		}
		idx1 = idx2;
	}
	return vect;
}

void main() {
	setlocale(LC_ALL, "Russian");

	int N;
	vector<int> v_1d; // введенные данные

	while (true) {
		cout << "Введите количество элементов (не менее 4-х): \n"; cin >> N;

		if (cin.fail()) { Clear_input("   неправильный ввод.\n\n"); continue; }
		else if (N < 4) { Clear_input("   число слишком маленькое.\n\n"); continue; }

		Clear_input("Введите сами элементы (целые числа) через пробел:");
		int new_element;
		for (int i = 0; i < N; i++) { cin >> new_element;  v_1d.push_back(new_element); }

		if (!cin.fail()) break;
		Clear_input("   неправильный ввод.\n\n");
	}

	int v2_size = sqrt(v_1d.size());
	int v2_lose = v_1d.size() - v2_size * v2_size;
	if (v2_lose > 0) cout << "Для двумерного массива проигнорируются последних чисел: " << v2_lose << endl;
	vector<vector<int>> v_2d;

	int z = 0;
	for (int y = 0; y < v2_size; y++) {
		vector<int> row_in_2d;
		for (int x = 0; x < v2_size; x++) {
			row_in_2d.push_back(v_1d[z]);
			z += 1;
		}
		v_2d.push_back(row_in_2d);
	}

	Vector_cout(BozoSort(v_1d), "\n");
	Vector_cout(BozoSort(v_1d, false), "\n");

	Vector_cout(BozoSort(v_2d));
	Vector_cout(BozoSort(v_2d, false));

	Vector_cout(BozoSort(v_1d[0], v_1d[1], v_1d[2]), "\n");
	Vector_cout(BozoSort(v_1d[0], v_1d[1], v_1d[2], false), "\n");
}
