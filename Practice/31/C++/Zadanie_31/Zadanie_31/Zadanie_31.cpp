#include <iostream>
#include <vector>
#include <fstream>
#include <string>
#include <filesystem>
using namespace std;
//
//
//
//
//################## Setup section #####################
//
// 1. Выбор типа передачи вектора: по ссылке или по значению
// Стоит передача по ссылке. 
#define peredacha_po_ssilke // закоментируйте #define для передачи ветора по значению


// 2. Взаимодействие с пайтоновским cfg.txt файлом
//
// Стоит получение настроек из конфига cfg.txt + проверяет и копирует, если его нет, EXE файл
#define data_get_from_config // закоментируйте для локального определения переменных


// !! Настройка возможна только, если путкт 2 закоментирован !!
// ==>  //#define data_get_from_config
// 3. Локальное определение размера вектора и .reserve
//

#ifndef data_get_from_config
// 2. vector reserve settings и количество передаваемых элементов
int reserv   = 0;   // 0-нет резерва.
int elements = 64;   // <0 - сразу выход
#endif

//################## END Setup section #####################
//
//
//
//
//

#ifdef peredacha_po_ssilke
	bool po_ssilke = true;
	ostream& operator<< (ostream & out, const vector<int> &vect) {
#else
	bool po_ssilke = false;
	ostream& operator<< (ostream & out, const vector<int> vect) {
#endif

	out << R"({'size':)" << vect.size() << R"(, 'capacity':)" << vect.capacity() << R"(, 'adr': [)";
	for (int i = 0; i < vect.size(); i++) 
		out << "0x" << &vect[i] << ((i<(vect.size()-1))?", ":"");
	out << "]},";
	return out;
}

int main()
{
// Чтение пайтоновского cfg с двумя переменными. Проверка наличия EXE файла, копирование, если его нет
#ifdef data_get_from_config
	int elements = 64;			// подгружаем дефолтные (по заданию) значения
	int reserv = 0;				// на тот случай, если Вы вдруг не перекопировали cfg.txt

	ifstream cfg("cfg.txt");
	if (cfg.is_open()) {		// читаем cfg.txt		
		string line;			// проверки, что в cfg.txt не то - нет.
		getline(cfg, line, '\n');
		int elements=stoi(line);
		getline(cfg, line, '\n');
		int reserv = stoi(line);
	}

	string tmp = ((po_ssilke) ? "_lnk" : "_val");		// создаём имя EXE файла
	string filename_exe = "Zadanie_31" + tmp + ".exe";

	if (!filesystem::exists(filename_exe)) {			// проверяем, если его нет - копируем в корень проекта
		try { filesystem::copy(R"(Debug\Zadanie_31.exe)", filename_exe); //filesystem::remove("from.txt");
		cout << filename_exe << " file copy ok.\n";
		}
		catch (filesystem::filesystem_error& e) {cout << e.what() << '\n';} // всё может быть
	}else {cout << filename_exe << " file already exists.\n";} // EXE файл уже есть
#endif

	// само задание: генерация и распечатка (формат вывода изменён для пайтона) вектора

	if (elements <= 0) return 0;
	vector<int> vect; 	if (reserv)	vect.reserve(reserv);
	string filename="data_e"+to_string(elements)+"_res"+to_string(reserv)+((po_ssilke)?"_lnk":"_val")+".txt";

	ofstream data_txt(filename);
	data_txt << "[\n[" << po_ssilke << ", " << reserv << ", " << elements << "],\n";

	for (int i = 0; i < elements; i++) {
		vect.push_back(i);
		data_txt << vect << endl;
	}
	while (vect.size()) {
		vect.pop_back();
		data_txt << vect << endl;
	}
	data_txt << "\n]";
	data_txt.close();
}
