//
// Извините.
// Сильно не доделано. 
// Пока только вникаю в с++ и как на нем что-то можно писать
// не извертываясь наизнанку.
//
// Сделано по с++ версии:
// 1. Поставил в VCInstallere ASP.net и вёб поддержку, включая json
// 2. Провел рассмотрение наиболее популярных библиотек для c++ json
// 3. Скачал с гит и добавил в проект библиотеку jsoncpp-master
// 4. Попробовал примитивное чтение и вывод на экран данных из файла in.json
// 5. 23:25 - нужно выкладывать хоть что-то на гит :)

#include <iostream>
#include <locale>
#include <json/value.h> //<json/value.h>  <jsoncpp/json/json.h><json/json.h>
#include <fstream>

using namespace std;


int main()
{
    setlocale(LC_ALL, "Russian");

    std::ifstream in_file("in.json", std::ifstream::binary);
    in_file >> jobs_done;

    //cout << jobs_done;

    cout << jobs_done["userId"];
    cout << jobs_done["title"];
    cout << jobs_done["completed"];

}
