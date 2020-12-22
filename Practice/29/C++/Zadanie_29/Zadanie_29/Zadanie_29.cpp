#include <iostream>
#include <string>
#include <vector>
#include <map>
#include <iomanip>
#include <cstdlib>
#include <ctime>
using namespace std;

struct Subj { string name; int mark; }; // ещё вариант

struct Student {
	string name = "";
	unsigned short group = 0;
	map <string, int> exams = { {	{"mathematics",0},
									{"physics",0},
									{"history",0},
									{"programming",0}} };
};

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

void prt(string s, int w) {
	out << setw(w) << left << s;
}

void operator<< (ostream& out, Student student) {
	out << "| " << setw(10) << left << student.name << "| ";
	out << setw(7) << left << student.group; out << "| ";
	out << setw(7) << left << student.exams["mathematics"];	out << "| "; out << setw(7) << left << student.exams["physics"];
	out << "| "; out << setw(7) << left << student.exams["history"]; out << "| "; out << setw(7) << left << student.exams["programming"];
	out << "|"; out << endl;
	out << "+-----------+-------+------+------+------+------+";	out << endl;
}



int main()
{
	srand(time(0));
	setlocale(LC_ALL, "");
	vector<Student> man;
	vector<int> randman;
	vector<string> names{ "Name1 S1","Name2 S2","Name3 S3","Name4 S4","Name5 S5",
						  "Name6 S6","Name7 S7","Name8 S8","Name9 S9","Name10 S10" };

	cout << "+-----------+-------+------+------+------+------+" << endl;
	cout << "| Name      | Group | Math | Phys | Hist | Prog |" << endl;
	cout << "+-----------+-------+------+------+------+------+" << endl;

	return 0;
}
