#include <iostream>
#include <cpp_httplib/httplib.h>
#include "include/nlohmann/json.hpp"
#include <string>
#include <fstream>
#include <time.h>
#include <stdio.h>
#include <locale>
#include <string>  //std::string s = std::to_string(some_int);

const auto html1 = R"(<!DOCTYPE html><html lang="en"><head><meta charset="UTF-8"><title>Weather Server</title></head><body>
"<div className = 'weather-icon'><img style = {{width:'70px'}} src = http://openweathermap.org/img/w/")";

const auto html2 = R"(".png /></div>"</body></html>)";

const std::string w_dir = "/data/2.5/forecast?id=693805";
const std::string w_opt = "&units=metric";                  //&lang=ru&exclude=current,minutely,daily,alerts
const std::string w_key = "&appid=b412de423d3145ef2e2941fa464b6aa1";


using json = nlohmann::json;
using namespace httplib;

std::string buffer("Hello, World!");
json w_json_data = json::array(); //std::string b = "  ";
json t_json_data = json::array();
json w_json_short = json::array();


int FindTimeInCash(int dt) {
    int search_idx = 0; // 0=ничего не нашли
    for (int i = 0; i < w_json_short.size(); i++) {
        if (dt <= w_json_short[i]["dt"].get<int>()) {
            search_idx = i + 1; // Ноль нужен для передачи сбоя
            break;
        }
    }

    return search_idx;
}

bool GetTime() {
    std::cout << "    +GetTime\n";
    Client cli("http://worldtimeapi.org");
    auto res = cli.Get("/api/timezone/Europe/Simferopol");

    if (res) {
        if (res->status == 200) {
            t_json_data = json::parse(res->body);

            //time_t epch = 1605820531; //0x45d732f6;
            //struct tm* timeinfo;
            //timeinfo = localtime(&rawtime);
            //std::cout << epch << " --- "; //<< asctime_s(gmtime(&epch));
            //printf("0x%x -> %s", epch, asctime(gmtime(&epch)));
            //time_t rawtime = (const time_t)**1370001284000 **;
            //struct tm* timeinfo;
            //timeinfo = localtime(&rawtime);
            //printf("Current local time and date: %s", asctime(timeinfo));
        }
        else { std::cout << "Status code: " << res->status << std::endl; }
        return true;
    }
    else { auto err = res.error(); std::cout << "Error code: " << err << std::endl; return false; }
}

bool GetWeather() {

    // с про он не дает почасовой. на обычном нет hourly. я так и не смог найти. 
    // берем 3-х часовой
    //
    //Client cli("http://pro.openweathermap.org"); // 
    //auto res = cli.Get("/data/2.5/forecast/hourly?id=693805&appid=b412de423d3145ef2e2941fa464b6aa1&units=metric&exclude=current,minutely,daily,alerts"); //&lang=ru

    Client cli("http://api.openweathermap.org");
    auto res = cli.Get("/data/2.5/forecast?id=693805&units=metric&appid=b412de423d3145ef2e2941fa464b6aa1");     //&lang=ru&exclude=current,minutely,daily,alerts

    if (res) {
        if (res->status == 200) {

            w_json_data = json::parse(res->body); //w_json_short   ["main"]["temp"]
            for (int i = 0; i < w_json_data["list"].size(); i++) {

                w_json_short.push_back({
                    {"dt"  ,        w_json_data["list"][i]["dt"].get<int>()             },
                    {"dt_txt",      w_json_data["list"][i]["dt_txt"]                    },
                    {"temp",        w_json_data["list"][i]["main"]["temp"].get<float>() },
                    {"temp_int",    w_json_data["list"][i]["main"]["temp"].get<int>()   },
                    {"description", w_json_data["list"][i]["weather"][0]["description"] },
                    {"city_name",   w_json_data["city"]["name"]                         },
                    {"icon",        w_json_data["list"][i]["weather"][0]["icon"]        } });
            }

        }
        else { std::cout << "Status code: " << res->status << std::endl; }
        return true;
    }
    else { auto err = res.error(); std::cout << "Error code: " << err << std::endl; return false; }
}

void gen_response_raw(const Request& req, Response& res) {
    std::cout << "     + gen_response_raw\n";

    if (!GetTime()) { // сервер времени спит
        std::cout << "Error! Текущее время недоступно! \n";
        res.set_content("Error! Текущее время недоступно!\n", "text/plain"); return;
    }
    // std::cout << "Текущее ю-время: " << t_json_data["unixtime"].get<int>() << "\n"; //std::cout << "Текущее время  : " << t_json_data["datetime"] << "\n\n";

    int idx = FindTimeInCash(t_json_data["unixtime"].get<int>());
    if (!idx) { // время не найдено. или погодосервер спит или устарел кэш пробуем ещё раз забрать погоду

        if (!GetWeather()) { // сервер погоды не ответил. выходим.
            std::cout << "Сервер погоды не отвечает. Попробуйте позже.\n";
            res.set_content("Error! Сервер погоды не отвечает!\n", "text/plain"); return;
        }

        idx = FindTimeInCash(t_json_data["unixtime"].get<int>()); // сервер ответил. дубль два поиск времени

        if (!idx) { // не нашло время после обновления! просто выходим...
            std::cout << "Error! Какая-то общая ошибка! \n";
            res.set_content("Error! Какая-то общая ошибка!\n", "text/plain"); return;
        }
    }

    // пока просто отдаем json 
        //res.set_content(w_json_short[idx - 1].dump(4) + "\n\n" + w_json_short.dump(4), "text/plain");
    res.set_content(w_json_short[idx - 1].dump(4) + "\n\n", "text/plain");

}

void gen_response(const Request& req, Response& res) {

    std::cout << "     + gen_response\n";


    if (!GetTime()) { // сервер времени спит
        std::cout << "Error! Текущее время недоступно! \n";
        res.set_content("Error! Текущее время недоступно!\n", "text/plain"); return;
    }
    // std::cout << "Текущее ю-время: " << t_json_data["unixtime"].get<int>() << "\n"; //std::cout << "Текущее время  : " << t_json_data["datetime"] << "\n\n";

    int idx = FindTimeInCash(t_json_data["unixtime"].get<int>());
    if (!idx) { // время не найдено. или погодосервер спит или устарел кэш пробуем ещё раз забрать погоду

        if (!GetWeather()) { // сервер погоды не ответил. выходим.
            std::cout << "Сервер погоды не отвечает. Попробуйте позже.\n";
            res.set_content("Error! Сервер погоды не отвечает!\n", "text/plain"); return;
        }

        idx = FindTimeInCash(t_json_data["unixtime"].get<int>()); // сервер ответил. дубль два поиск времени

        if (!idx) { // не нашло время после обновления! просто выходим...
            std::cout << "Error! Какая-то общая ошибка! \n";
            res.set_content("Error! Какая-то общая ошибка!\n", "text/plain"); return;
        }
    }

    // отдаем виджет
    //"http://openweathermap.org/img/w/" +data['weather'][0]['icon']+ ".png' alt='Icon depicting current weather.'>"

    //std::string b = std::to_string(w_json_short[idx - 1]);

    //std::string b = w_json_short[idx - 1];
    //std::string e = html1 + html1; // хз как их склеить

    //b = html1 + b + html1;

    //res.set_content(html1 + html2, "text/plain");
    //std::cout << html1 << html2 << "\n";
    return;
}



int main() {
    setlocale(LC_ALL, "Russian");
    if (!GetWeather()) { std::cout << "Error get weather data. Server halted\n"; return 0; }

    Server svr;
    svr.Get("/raw", gen_response_raw);
    //svr.Get("/", gen_response);

    std::cout << "Weather server started\n";

    svr.listen("localhost", 1234); // supports  http://localhost:1234/
    return 0;
}

