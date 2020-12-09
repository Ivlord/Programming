#define DEBUG_ON
#include <iostream>
#include "cpp_httplib/httplib.h"
#include "nlohmann/json.hpp"
//#include <stdio.h>  // Для printf
#include <fstream>
#include <stdio.h>
#include <locale>
#include <string>  //std::string s = std::to_string(some_int);

char w_ip[] = "http://api.openweathermap.org";  //"/data/2.5/forecast?id=693805";
const std::string w_dir = "/data/2.5/onecall?lat=44.9572&lon=34.1108";
const std::string w_opt = "&lang=ru&units=metric&exclude=current,minutely,daily,alerts";
const std::string w_key = "&appid=b412de423d3145ef2e2941fa464b6aa1";

char t_ip[] = "http://worldtimeapi.org";
const std::string t_dir = "/api/timezone/Europe/Simferopol";

std::string wiget_file_name = R"(.\include\iframe.html)";
std::string wiget_temp = "{hourly[i].temp}";
std::string wiget_icon = "{hourly[i].weather[0].icon}";
std::string wiget_description = "{hourly[i].weather[0].description}";

const std::string err_Tsrv = "Ошибка! Текущее время недоступно!";  // Ошибка сервера времени
const std::string err_Wsrv = "Ошибка! Данные погоды недоступны!";  // Ошибка сервера погоды
const std::string err_Gen  = "Ошибка! Данные не найдены!";         // Ошибка повторного поиска

using json = nlohmann::json;
using namespace httplib;   //std::string buffer("Hello, World!");

json w_json_data; json t_json_data;
json w_json_short; // = json::object();

int SiteAccess(std::string dir, char ip[], std::string ident, json& result) {
    #ifdef DEBUG_ON
    std::cout << "  + запрос " + ident + "\n";
    #endif
    Client some_site(ip); auto res = some_site.Get(dir.c_str());

    if (res) {
        #ifdef DEBUG_ON
        std::cout << "   > Статус запроса " << ident << ": " << res->status << std::endl;
        #endif
        if (res->status == 200) {
            result = json::parse(res->body);
            return (int)res->status;
        }
        else { return (int)res->status; }
    }
    else {
        auto err = res.error();
        #ifdef DEBUG_ON
        std::cout << "   > Код ошибки " << ident << ": " << err << std::endl;
        #endif
        return (int)err;
    }
}

int GetJsonTime(){
    return (t_json_data.is_null()) ? 0 : t_json_data["unixtime"].get<int>();
    //if (t_json_data.is_null()) return 0;
    //return t_json_data["unixtime"].get<int>();
}

int FindTimeInCash(int cur_time) {
    if (w_json_data.is_null()) return 0;
    int max_idx  = w_json_data["hourly"].size()-1;
    int max_time = w_json_data["hourly"][max_idx]["dt"].get<int>();
    max_idx      = 1 + max_idx - (max_time - cur_time) / 3600; // idx
    return (cur_time> max_time || max_idx<0)? 0 : max_idx;
}

std::string PrepareWiget() {

    std::ifstream inFile(wiget_file_name); // Файловая переменная
    std::string one_line;
    if (inFile.is_open()) {
        //std::getline(inFile, one_line);
        getline(inFile, one_line, '\0');
        inFile.close();
        //std::cout << one_line << "\n";
    }
    else {
        std::cout << "Ошибка открытия файла: " << wiget_file_name << "\n"; return one_line;
    }

    std::size_t found = one_line.find(wiget_temp);
    if (found != std::string::npos)
    {
        one_line.replace(found, wiget_temp.size(), std::to_string(w_json_short["Reply"]["temp"].get<int>()));
    }

    found = one_line.find(wiget_icon);
    if (found != std::string::npos)
    {
        one_line.replace(found, wiget_icon.size(), w_json_short["Reply"]["icon"].get<std::string>());
    }

    found = one_line.find(wiget_description);
    if (found != std::string::npos)
    {
        one_line.replace(found, wiget_description.size(), w_json_short["Reply"]["description"].get<std::string>());
    }

    //std::cout << one_line << "\n";

    return one_line;
}

bool gen_response_common() {
    std::cout << "  + gen_common\n";

    w_json_short = json::object();

    int t_res = SiteAccess(t_dir, t_ip, "времени", t_json_data);
    w_json_short.push_back({ "t_srv_code", t_res});
    if (t_res != 200) {
        w_json_short.push_back({ "unixtime", 0 }); w_json_short.push_back({ "w_srv_code", -1 });
        std::cout << w_json_short.dump(4) << "\n"; return false; }
    
    w_json_short.push_back({ "unixtime", GetJsonTime() });

    int idx = FindTimeInCash(GetJsonTime());
    if (!idx) { // время не найдено. или погодосервер спит или устарел кэш пробуем ещё раз забрать погоду
        int w_res = SiteAccess(w_dir + w_opt + w_key, w_ip, "погоды", w_json_data);
        w_json_short.push_back({ "w_srv_code", w_res });

        if (w_res != 200) { // сервер погоды не ответил. выходим.
            std::cout << w_json_short.dump(4) << "\n"; return false; }

        idx = FindTimeInCash(GetJsonTime());
        if (!idx) { // не нашло время после обновления! просто выходим...
            w_json_short.push_back({ "g_err_code", -1 });
            std::cout << w_json_short.dump(4) << "\n"; return false; }
    }
    w_json_short.push_back({ "Reply", w_json_data["hourly"][idx - 1]["weather"][0] });
    w_json_short["Reply"].push_back({ "temp"     , w_json_data["hourly"][idx - 1]["temp"].get<float>() });
    return true;
}

void gen_response_root(const Request& req, Response& res) {
#ifdef DEBUG_ON 
    std::cout << "  + gen_root\n";
#endif
    // отдаем виджет
    if (gen_response_common()) { 
        res.set_content(PrepareWiget(), "text/html");
    }//Error 
    else { res.set_content(w_json_short.dump(4) + "\n", "text/html"); }

    return;
}

void gen_response_raw(const Request& req, Response& res) {
#ifdef DEBUG_ON 
    std::cout << "  + gen_raw\n";
#endif
    if (gen_response_common()) {
        res.set_content(w_json_short["Reply"].dump(4) + "\n", "text/plain");
    }
    else { res.set_content(w_json_short.dump(4) + "\n", "text/plain"); }
}

void gen_response_full(const Request& req, Response& res) {
#ifdef DEBUG_ON 
    std::cout << "     + gen_full\n";
#endif
    if (gen_response_common()) {
        res.set_content(w_json_data.dump(4) + "\n", "text/plain");
    }
    else { res.set_content(w_json_short.dump(4) + "\n", "text/plain"); }
}

void gen_response_time(const Request& req, Response& res) {
    #ifdef DEBUG_ON 
    std::cout << " + gen_time\n";
    #endif
    int t_res = SiteAccess(t_dir, t_ip, "времени", t_json_data);
    if (t_res == 200) res.set_content(t_json_data.dump(4) + "\n", "text/plain");
    else res.set_content(w_json_short.dump(4), "text/plain");
}

int main() {
    setlocale(LC_ALL, "Russian");

    Server svr;
    svr.Get("/", gen_response_root);
    svr.Get("/raw", gen_response_raw);
    svr.Get("/full", gen_response_full);
    svr.Get("/time", gen_response_time);
    svr.Get("/stop", [&](const Request& req, Response& res) {
        std::cout << "Сервер остановлен по запросу пользователя\n";
        res.set_content("Сервер остановлен по запросу пользователя\n", "text/plain");
        svr.stop(); });
    //svr.Get("/refresh"  , [&](const Request& req, Response& res) { 
    //    if (!GetWeather()) { std::cout << err_Wsrv + " Сервер остановлен\n";
    //    res.set_content(err_Wsrv+"Сервер остановлен\n", "text/plain");
    //    svr.stop(); } 
    //    res.set_content("Данные погоды загружены заново\n", "text/plain");
    //    });

    std::cout << "Сервер погоды запущен.\n";

    svr.listen("localhost", 3000); // supports  http://localhost:1234/
    return 0;
}




int GetTime2() {
    Client time_server(t_ip); auto res = time_server.Get(t_dir.c_str());

    if (res) { // Сервер ХОТЬ что-то ответил!
        if (res->status == 200) {
            t_json_data = json::parse(res->body);
            return t_json_data["unixtime"].get<int>();
        }
        else {
            std::cout << "   > Статус запроса времени: " << res->status << std::endl;
            return int(res->status);
        }
    }
    else {
        auto err = res.error();
        std::cout << "   > Код ошибки времени: " << err << std::endl;
        return (int)err;
    }
}

int GetWeather2() {
    Client weather_server(w_ip); auto res = weather_server.Get((w_dir + w_opt + w_key).c_str());

    if (res) {
        if (res->status == 200) {
            w_json_data = json::parse(res->body);
            return (int)res->status;
        }
        else {
            std::cout << "   > Статус запроса погоды: " << res->status << std::endl;
            return (int)res->status;
        }
    }
    else {
        auto err = res.error();
        std::cout << "   > Код ошибки погоды: " << err << std::endl;
        return (int)err;
    }
}
