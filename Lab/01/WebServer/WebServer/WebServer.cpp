#include <iostream>
#include <cpp_httplib/httplib.h>
#include <nlohmann/json.hpp>
#include <string>
#include <fstream>

using json = nlohmann::json;
using namespace httplib;

std::string buffer("Hello, World!");
json w_json_data = json::array();;

void gen_response(const Request& req, Response& res) {
	res.set_content(w_json_data.dump(4), "text/plain");
}

bool GetWeather() {
    //Client cli("http://worldtimeapi.org");

    Client cli("http://api.openweathermap.org");

    auto res = cli.Get("/data/2.5/forecast?id=693805&appid=b412de423d3145ef2e2941fa464b6aa1");

    if (res) {
        if (res->status == 200) {
            //std::cout << res->body << std::endl; // В res->body лежит string с ответом сервера
            w_json_data = json::parse(res->body);
        }
        else { std::cout << "Status code: " << res->status << std::endl; }
        return true;}
    else
    {auto err = res.error(); std::cout << "Error code: " << err << std::endl; return false;}
}

int main() {
    
    if (!GetWeather()) {std::cout << "Error get data. Server halted\n";}

	Server svr;
	svr.Get("/", gen_response);
	std::cout << "Weather server started\n";

	svr.listen("localhost", 1234);
    return 0;
}

