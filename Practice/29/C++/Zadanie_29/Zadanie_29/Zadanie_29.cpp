#include <iostream>
#include <string>
#include <vector>
#include <iomanip>
using namespace std;

struct Student { string name = ""; int group = 0; int mathematics;	int physics; int history; int programming; };
bool operator< (Student& a, Student& b) {
	return a.name < b.name;
}
bool operator> (Student& a, Student& b) {
	return a.name > b.name;
}

template<typename T>
ostream& prt_l(ostream& out, T data, int width, string fin = "") {
	out << setw(width) << left << data << fin;
	return out;
}
ostream& operator<< (ostream& out, Student student) {
	out << "| ";
	prt_l(out, student.name, 12, "| ");
	prt_l(out, student.group, 6, "| ");
	prt_l(out, student.mathematics, 5, "| ");
	prt_l(out, student.physics, 5, "| ");
	prt_l(out, student.history, 5, "| ");
	prt_l(out, student.programming, 5, "| ");
	return out;
}
ostream& operator<< (ostream& out, vector<Student> student_list) {
	for (auto student : student_list) {
		out << student << "\n";
	}
	return out;
}

template <typename T>
void Bozo_sort_ar(vector<T>& vect, bool sort_type = true) {

	srand(time(NULL));
	int idx1 = rand() % vect.size(), idx2 = rand() % vect.size();
	T temporary;
	bool need_sorting = true;

	while (need_sorting) {
		while (idx1 == idx2) {
			idx1 = rand() % vect.size();
			idx2 = rand() % vect.size();
		}
		temporary = vect[idx1]; vect[idx1] = vect[idx2]; vect[idx2] = temporary;

		need_sorting = false;
		for (unsigned i = 1; i < vect.size(); i++) {

			if ((sort_type && (vect[i - 1] > vect[i])) || (!sort_type && (vect[i - 1] < vect[i]))) {
				need_sorting = true; break;
			}
		}
		idx1 = idx2;
	}
}

string footer = "+-------------+-------+------+------+------+------+\n";
string header = footer + "| Name        | Group | Math | Phys | Hist | Prog |\n" + footer;

int main() {
	setlocale(LC_ALL, ""); srand(time(NULL));

	vector<Student> students, assholes; // двоешники не сдавшие программирование
	vector<string> first_names{ "Akihiko", "Carmack", "Kojima", "Persson", "Molyneux", "Meier", "Newell",
	  "Gates", "Zukerman", "Lopez", "Dolores", "Kim", "Mazlov", "Riply", "Connor", "Norris", "Li", "Harven" };
	string second_names = "ABCDEFGHIJKLMNOPQRSTUVWXYZ";
	for (short i = 0; i < 10; i++) {
		students.push_back({ first_names[rand() % first_names.size()] + " " + second_names[rand() % second_names.size()],
			1 + rand() % 9, 2 + rand() % 4, 2 + rand() % 4, 2 + rand() % 4, 2 + rand() % 4 });
		if ((students[i].mathematics == 2) || (students[i].physics == 2) || (students[i].history == 2) || (students[i].programming == 2)) {
			assholes.push_back(students[i]);
		}
	}
	Bozo_sort_ar(students, true); //cout << header << students << footer << "\n"; // несортированный вывод
	cout << header << students << footer << "\nExpulsion\n\n";				      // сортированный список студентов

	if (assholes.size()) cout << header << assholes[rand() % assholes.size()] << "\n" << footer << "\n";
	else cout << "Not found\n";
	return 0;
}
