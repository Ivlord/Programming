#include <iostream> //#include <cstdlib>
#include <string>
#include <vector>
#include <fstream>
using namespace std;

string commas = R"("")";

struct Passenger {
	int passengerid;
	bool survival;
	int pclass;
	string name;
	string sex;
	double age;
	int sibsp;
	int parch;
	string ticket;
	double fare;
	string cabin;
	string embarked;
	int original_passengerid; // поле добавлено для более наглядного вывода отсортированного
};							  // списка alive_femeles_1 в отдельный файл "females.csv"

ostream& operator<< (ostream& out, vector<Passenger>& vect) {
	out << "PassengerId, Survived, Pclass, Name, Sex, Age, SibSp, Parch, Ticket, Fare, Cabin, Embarked" << '\r';
	for (size_t i = 0; i < vect.size(); i++)
	{
		out << vect[i].passengerid << ",";
		out << vect[i].survival << ",";
		out << vect[i].pclass << ",";
		out << vect[i].name << ",";
		out << vect[i].sex << ",";
		out << vect[i].age << ",";
		out << vect[i].sibsp << ",";
		out << vect[i].parch << ",";
		out << vect[i].ticket << ",";
		out << vect[i].fare << ",";
		out << vect[i].cabin << ",";
		out << vect[i].embarked << '\r';
	}
	return out;
}

ostream& operator<< (ostream& out, vector<Passenger*> vect) { // сделано для удобной проверки
	// сбрасывает вектор указателей отсортированных "survival female SES-1" в females.csv
	for (size_t i = 0; i < vect.size(); i++) {
		out << (*vect[i]).passengerid << ",";
		out << "<-id-change-" << (*vect[i]).original_passengerid << ",";
		out << (*vect[i]).survival << ",";
		out << (*vect[i]).pclass << ",";
		out << (*vect[i]).name << ",";
		out << (*vect[i]).sex << ",";
		out << (*vect[i]).age << ",";
		//out << (*vect[i]).sibsp << ","; // убрал, чтобы не перегружать вывод излишней инфой
		//out << (*vect[i]).parch << ",";
		//out << (*vect[i]).ticket << ",";
		//out << (*vect[i]).fare << ",";
		//out << (*vect[i]).cabin << ",";
		//out << (*vect[i]).embarked ;
		out << '\r';
	}return out;
}

string cut(string& line, string repl = "", bool y = false) {
	string ret = "";
	if (y) { ret = line.substr(0, line.find(',')); line.erase(0, line.find(',') + 1); }
	else { ret = line.substr(line.rfind(',') + 1); line.erase(line.rfind(',')); }
	return ((ret.size()) ? ret : repl);
}

Passenger& operator+ (Passenger& p, string& line) {
	p.passengerid = stoi(cut(line, "0", true)); // отрезаем первых цифры с начала строки
	p.survival = !!stoi(cut(line, "0", true)); // до начала имени
	p.pclass = stoi(cut(line, "1", true));
	p.embarked = cut(line);				// режем с конца-чтобы осталось только имя
	p.cabin = cut(line);				// идём к имени с конца
	p.fare = stod(cut(line, "0.0"));		// 
	p.ticket = cut(line);
	p.parch = stoi(cut(line, "0"));
	p.sibsp = stoi(cut(line, "0"));
	p.age = stod(cut(line, "0.0"));
	p.sex = cut(line);				// осталось только имя
	if (line[0] == '"')							line.erase(0, 1);
	if (line[line.size() - 1] == '"')			line.erase(line.size() - 1, 1);
	while (line.find(commas) != string::npos)	line.erase(line.find(commas), 1);
	p.name = line;
	p.original_passengerid = p.passengerid; // для отслеживания изменения id в доп. отчёте
	return p;
}

istream& operator>> (istream& in_stream, vector<Passenger>& vect) { //size_t p = string::npos;
	string line;
	Passenger one_person;
	while (getline(in_stream, line, '\r')) {
		if (line.find("PassengerId") != string::npos) continue;
		vect.push_back(one_person + line);
	}
	return in_stream;
}

bool operator< (Passenger a, Passenger b) { // для сортировки пришлось перегружать < и > и ==
	return a.age < b.age;
}
bool operator> (Passenger a, Passenger b) { return a.age > b.age; }
bool operator== (Passenger a, string b) { return a.name == b; }

template <typename T>
int sorter(vector<T*> vect, bool sort_type = true) { // сортировка: неоптимизированный пузырёк
	T tmp;
	int cnt = 0, id_swap = 0;
	bool need_sort = true;
	while (need_sort) {
		need_sort = false;
		for (auto i = 0; i < vect.size() - 1; i++) {
			if ((sort_type && (*vect[i] < *vect[i + 1])) || (!sort_type && (*vect[i] > * vect[i + 1]))) {
				tmp = *vect[i]; *vect[i] = *vect[i + 1]; *vect[i + 1] = tmp; // a,b=b,a
				id_swap = (*vect[i]).passengerid;  // меняем отдельно порядковый номер
				(*vect[i]).passengerid = (*vect[i + 1]).passengerid;
				(*vect[i + 1]).passengerid = id_swap;
				need_sort = true; cnt++;
			}
		}
	}return cnt;
}

int main() {
	ifstream train("train.csv"); 			vector<Passenger> titanic;
	if (!train.is_open()) return 0;
	train >> titanic;						vector<Passenger*> alive_femeles_1;

	for (size_t i = 0; i < titanic.size(); i++) {
		if (titanic[i].pclass == 1 && titanic[i].sex == "female" && titanic[i].survival)
			alive_femeles_1.push_back(&titanic[i]);
	}

	int sort_iterations = sorter(alive_femeles_1, false);
	ofstream save_file("save.csv");
	save_file << titanic;					save_file.close();

#define TEST_MODE  // режим дополнительной проверки и справки .
#ifdef TEST_MODE
	//ofstream fem_save_file("females.csv");
	//fem_save_file << alive_femeles_1;		fem_save_file.close();

	ofstream fem_save_file("females.csv");
	fem_save_file << alive_femeles_1;		fem_save_file.close();


	string hollywood_vip = "Leonardo DiCaprio";
	bool find_hero = false;
	for (auto i : titanic) {									// scan full list
		if (i == hollywood_vip) { find_hero = true; break; }
	}

	if (!find_hero) {
		cout << "Scan statistics:\n   Total passengers scaned:      " << titanic.size() << "\n";
		cout << "   Alive Females SES-1 detected: " << alive_femeles_1.size() << "\n";
		cout << "   Sort iterations done        : " << sort_iterations << "\n";
		cout << "\n\n... but \n\n!!!Warning!!!\n\n" + hollywood_vip + " has not been found.\nThis data may be incomplete or fake.\n\nWe know for sure that he was there and died tragically.\n\n\n";
	}

#endif
	return 0;
}
