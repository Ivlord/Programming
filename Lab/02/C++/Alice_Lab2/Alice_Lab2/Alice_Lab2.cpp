#include <iostream>
#include <cpp_httplib/httplib.h>
#include <nlohmann/json.hpp>
#include <vector>
#include <string>
#include <iostream>
#include <fstream>
//#include <cmath>
//#include <iomanip>

using namespace std;
using namespace httplib;
using json = nlohmann::json;

string WHConfig_filename = "Webhooks_Cfg.json";
string WHT_html_filename = "webhooks_template.html";
string WH_pattern = "{webhooks_list}";
string del1 = u8R"(<div class = "form-row align-items-center"><div class = "col"><input type = "text" value = ")";
string del2 = u8R"(" class = "form-control mb-2" disabled></div><div class = "col"><button type = "submit" name = "del" value = ")";
string del3 = u8R"(" class = "btn btn-danger mb-2">Удалить</button></div></div>)";

json cfg;


//"url": "https://example.com/",
void gen_response(const Request& req, Response& res) {

    ofstream logger("log.txt");


    //cout << req.body.c_str()<<"\n\n";

    json body = json::parse(req.body.c_str()); //u8
    cout << body.dump(4) << "\n\n==================================================================\n\n";
    logger << body.dump(4) << "\n\n==================================================================\n\n";


    std::string str = u8R"(
    {
      "response": {
        "text": "Здравствуйте! Я помогу вам с покупками.",
        "tts": "Здравствуйте! Я помогу вам с покупками.",
        "buttons": [
            {
                "title": "Помощь",
                "payload": {},
                "hide": true
            },
            {
                "title": "Молчать",
                "payload": {},
                "hide": true
            }
        ],
        "end_session": false
      },
      "version": "1.0"
    })";
    res.set_content(str, "text/json; charset=UTF-8");
}

bool ConfigSave(json& cfg) {
    ofstream cfg_file(WHConfig_filename);
    if (!cfg_file.is_open()) {
        cout << "failed to save.\n";    // some file create error
        return false;
    }

    cfg_file << cfg; cfg_file.close(); // {"webhooks":["site1.com","site2.com"]}
    cout << "done.\n";
    return true;
}

// Checks Webhooks_Cfg.json, reads it or creates empty one. 
bool ConfigCheck(json& cfg) {
    cout << "> Checking config file ... ";
    ifstream cfg_file(WHConfig_filename);
    if (!cfg_file.is_open()) {
        cout << "not found ... creating new ... ";                // no config -> create new -> fill empty
        cfg = json::parse(R"({"webhooks": []})");
        return ConfigSave(cfg);
    }

    cout << "reading ... "; cfg_file >> cfg; // cfg found and opened
    cfg_file.close(); cout << "done.\n";
    return true; // cout << cfg.dump(4);
}

void gen_response_WHGetPost(const Request& req, Response& res) {
    if (!ConfigCheck(cfg)) {
        res.set_content(u8"Какие-то проблемы с файлом конфига.", "text/html; charset=UTF-8");
        return;
    }

    if (req.method == "POST") {
        if (req.has_param("del")) {
            auto site_to_del = req.get_param_value("del");
            for (int i = 0; i < cfg["webhooks"].size(); i++)
            {
                cout << "   json(site_to_del)=" << json(site_to_del) << endl;
                cout << "   i = " << i << " " << cfg["webhooks"][i] << endl;

                if (cfg["webhooks"][i] == site_to_del) {
                    cfg["webhooks"].erase(i); ConfigSave(cfg);
                    cout << "   del> i = " << i << " " << cfg["webhooks"][i] << endl;
                }
            }
        }

        if (req.has_param("set")) {
            auto val_set = req.get_param_value("set");
            cfg["webhooks"].push_back(val_set); ConfigSave(cfg);
        }
    }

    cout << cfg["webhooks"].size() << "\n" << cfg.dump(4) << endl;

    ifstream WH_html(WHT_html_filename);
    if (!WH_html.is_open()) {
        cout << WHT_html_filename << " not found. Get request to /webhooks failed.\n"; return;
    }

    string WH_template_text; getline(WH_html, WH_template_text, '\0'); WH_html.close(); // сборка вставки в reply
    string reply_insert;

    for (int i = 0; i < cfg["webhooks"].size(); i++) {
        reply_insert.append(del1).append(cfg["webhooks"][i]).append(del2).append(cfg["webhooks"][i]).append(del3);
    } //for (int i = 0; i < stro.length(); i++){if (stro[i] == '\\'){stro.erase(i);}}

    WH_template_text.replace(WH_template_text.find(WH_pattern), size(WH_pattern), reply_insert);
    res.set_content(WH_template_text, "text/html; charset=UTF-8");  // ответ на Get запрос
}

int main() {
    ConfigCheck(cfg);

    Server svr;
    svr.Post("/", gen_response);
    svr.Get("/webhooks", gen_response_WHGetPost);
    svr.Post("/webhooks", gen_response_WHGetPost);
    std::cout << "Start server... OK\n";
    svr.listen("localhost", 3000);
}
