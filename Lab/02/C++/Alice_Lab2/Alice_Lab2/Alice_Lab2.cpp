#include <iostream> //#include <cmath> //#include <iomanip>
#include <cpp_httplib/httplib.h>
#include <nlohmann/json.hpp>
#include <vector>
#include <string>
#include <iostream>
#include <fstream>
using namespace std;
using namespace httplib;
using json = nlohmann::json;

string Dialog_filename = "Dialog.json";
string WHConfig_filename = "Webhooks_Cfg.json";
string WHT_html_filename = "webhooks_template.html";
string WH_pattern = "{webhooks_list}";

string del1 = u8R"(<div class = "form-row align-items-center"><div class = "col"><input type = "text" value = ")";
string del2 = u8R"(" class = "form-control mb-2" disabled></div><div class = "col"><button type = "submit" name = "del" value = ")";
string del3 = u8R"(" class = "btn btn-danger mb-2">Удалить</button></div></div>)";

json j_shablon_btn = json::parse(u8R"({ "title": " ", "hide" : true })"); //"payload": {},
json j_shablon_rep = json::parse(u8R"({ "response": { "text": " ", "tts": " ", "buttons": [],
                                        "end_session": false }, "version": "1.0"})");
// "silent_mode"    - 0- Ализа заткнулась,  1 - продолжает говорить 
// "screen":"Main"  - текущий экран
json ses_shablon = json::parse(u8R"({"screen":"Main", "silent_mode":1,  
                                     "session_id":"", "user_id":"" , "basket": { "user_id": "", "check" : [] } } )");

json good = json::parse(u8R"({ "item": "", "price" : 0 })");
//json basket = json::parse(u8R"({ "user_id": "", "check" : [] })");

json cfg, ses; // config file and session json
json button_exception = {}, s = {}, a = {}; // short names for dialog parts: button_exception, screens, actions

void ppp(json& data, bool yy = true) {
    ofstream logger("log.txt");
    logger << data.dump(1) << "\n==================================================================\n\n\n";
    if (yy) logger.close();
}
void pps(string& data, bool yy = true) {
    ofstream logger("log.txt");
    logger << data << "\n==================================================================\n\n\n";
    if (yy) logger.close();
}

bool exists(const json& j, const string& key) { // проверка, есть ли ключ в словаре
    return j.find(key) != j.end();
}

void MakeBtns(json& list_for_buttons, json& need_button_list) {
    for (unsigned i = 0; i < need_button_list.size(); i++) {
        string new_button_name = need_button_list[i];

        if (exists(button_exception, new_button_name)) {
            if (button_exception[new_button_name]["silent_mode"] == ses["silent_mode"]) continue;
        }
        json new_button = j_shablon_btn;
        new_button["title"] = new_button_name;
        list_for_buttons.push_back(new_button);
    }
}

json MakeReply(string txt, string tts, bool end_session = false) {
    json reply = j_shablon_rep;
    string tmp = ses["screen"];
    if (!exists(s, ses["screen"])) ses["screen"] = "Main";

    MakeBtns(reply["response"]["buttons"], s[tmp]["buttons"]);

    if (ses["silent_mode"] == 0) tts = "sil <[500]>";
    reply["response"]["text"] = txt;
    reply["response"]["tts"] = tts;
    reply["response"]["end_session"] = end_session;
    //ppp(reply);
    return reply;
}

bool In_str(string main_string, string sub_string) {
    // возвращает true, если подстрока найдена в строке
    return (main_string.find(sub_string) != std::string::npos) ? true : false;
}

int CntToken(string data) {
    int cnt = 1;                                 // points to the next token
    while (data.find(" ") != std::string::npos) {
        cnt += 1;
        data.replace(data.find(" "), size(" "), "");
    }
    return cnt;
}

bool SendWebhooks(json& basket) {
    for (int i = 0; i < cfg["webhooks"].size(); i++) {

        string webhook_url = cfg["webhooks"][i]; // https:\\ 9 chars

        string url = webhook_url.substr(0, webhook_url.find("/", 9));  // url
        string dir = webhook_url.substr(webhook_url.find("/", 9));                     // dir

        Client cli(url.c_str());
        auto cli_res = cli.Post(dir.c_str(), basket.dump(), "application/json");

        if (cli_res) { // Проверяем статус ответа, т.к. может быть 404 и другие
            if (cli_res->status == 200) {
                // В res->body лежит string с ответом сервера
                //std::cout << cli_res->body << std::endl;
                return true;
            }
            else  std::cout << "Status code: " << cli_res->status << std::endl;
        }
        else { auto err = cli_res.error();  std::cout << "Error code: " << err << std::endl; }
        return false;
    }
}

//["session"]["user"]["user_id"] or "anonymous"
json OrderProcess(json& body) {
    string msg_picker = "";                   // ключ правильного сообщения
                                              // Инициализация новой сессии с приветственным ответом. +обнуление корзины.
    if (body["session"]["new"].get<bool>()) { // true-для новой сесии, false- продолжение
        ses = ses_shablon;
        ses["session_id"] = body["session"]["session_id"];            // сохраняем новый id сессии

        if (empty(body["session"]["user"]["user_id"])) ses["user_id"] = "anonymous";
        else ses["user_id"] = body["session"]["user"]["user_id"];       // и id пользователя или "anonymous"
        msg_picker = ses["screen"];

        return MakeReply(s[msg_picker]["in_msg"][0], s[msg_picker]["in_tts"][0]);
    }
    else if (body["session"]["session_id"] != ses["session_id"])    // пришла сессия без id?
      // сессия поменяла свой id неожиданно?
    { // резко завершаем разговор и валим закрываем сессию=true
        msg_picker = ses["screen"];
        return MakeReply(s["Main"]["out_msg"][0], s["Main"]["out_tts"][0], true);
    }

    string msg_out = "", tts_out = "";
    string fnd_word = "", fnd_act = "";  // для совпавшей action и конкретно совпавшего слова

    string current_screen = ses["screen"];
    //for (auto& action : s[current_screen]["actions"]) { // идём по списку возможных реакций на текущем screen

    json sa = s[current_screen]["actions"];
    for (unsigned i = 0; i < sa.size(); i++) { // список экшинов на экране
        string new_chck = sa[i]; // конкретный экшин

        if (!exists(a, new_chck)) continue;              // такой реакции нет в списке реакций. пропускаем.
        for (unsigned j = 0; j < a[new_chck]["check"].size(); j++) { // идём оп словам контрольным

            if (!In_str(body["request"]["original_utterance"], a[new_chck]["check"][j])) continue;// нет совпадения - дальше
            fnd_act = new_chck;                         // запоминаем "сработавший" action
            fnd_word = a[new_chck]["check"][j];         // и конкретное слово, на котором сработало
            break;
        } if (fnd_act != "") break;
    }

    if (empty(fnd_act)) { // никаких признаков правильного ввода от пользователя, возвращаем "я не знаю такой команды" // доделать
        return MakeReply(s[(string)ses["screen"]]["fail_msg"][0], s[(string)ses["screen"]]["fail_tts"][0]);
    }

    //// command processing section
    for (auto& com : a[fnd_act]["react"][(string)ses["screen"]]) { // com= [{"CgangeVar":"Can_talk", "Val":false}, {"Msg":"Start"}]
        if (exists(com, "Msg")) {              // обработка команд Msg: устанавливаем текст ответа
            msg_picker = com["Msg"];           // берём название текста, для печати из значения команды "Msg"
            msg_out = msg_out + a[fnd_act]["messages"][msg_picker]["msg"][0].get<string>();
            tts_out = tts_out + a[fnd_act]["messages"][msg_picker]["tts"][0].get<string>();
            continue;
        }
        else if (exists(com, "ChangeVar_Can_talk")) {
            ses["silent_mode"] = com["ChangeVar_Can_talk"].get<int>();
        }
        else if (exists(com, "ChangeVar_screen")) {
            ses["screen"] = com["ChangeVar_screen"].get<string>();
        }
        else if (exists(com, "print")) {} // так и надо
        else if (exists(com, "del_good")) { //"del_good""add_good""sum""dell_all""list"
            string new_good = body["request"]["nlu"]["tokens"][CntToken(fnd_word)];
            bool fnd_good = false;
            for (unsigned i = 0; i < ses["basket"]["check"].size(); i++) {
                if (ses["basket"]["check"][i]["item"] != new_good) continue;
                ses["basket"]["check"].erase(i);
                fnd_good = true;
                break;
            }
            msg_picker = "Fail";
            if (fnd_good) msg_picker = "Main";
            msg_out = msg_out + a[fnd_act]["messages"][msg_picker]["msg"][0].get<string>();
            tts_out = tts_out + a[fnd_act]["messages"][msg_picker]["tts"][0].get<string>();
        }
        else if (exists(com, "list")) { // List of goods

            int cnt = 0;
            bool first_run = true;
            for (unsigned i = 0; i < ses["basket"]["check"].size(); i++) {
                if (first_run) {
                    msg_out += string(a[fnd_act]["messages"]["Main"]["msg"][0]);
                    tts_out += string(a[fnd_act]["messages"]["Main"]["tts"][0]);
                    first_run = false;
                }
                string m1 = string(ses["basket"]["check"][i]["item"]); //.get<string>()
                string m2 = to_string(ses["basket"]["check"][i]["price"]);
                msg_out = msg_out + m1 + "\t" + m2 + "\n";
                tts_out = msg_out + m1 + "\t" + m2 + "\n";
                cnt += 1;
            }

            if (cnt == 0) { // если не найдено в корзине
                msg_out = msg_out + string(a[fnd_act]["messages"]["Fail"]["msg"][0]);
                tts_out = tts_out + string(a[fnd_act]["messages"]["Fail"]["tts"][0]);
            }
        }
        else if (exists(com, "sum")) { // Sum
            int price = 0;
            for (unsigned i = 0; i < ses["basket"]["check"].size(); i++) {
                price = price + ses["basket"]["check"][i]["price"].get<int>();
            }

            if (price != 0) { // есть покупки
                msg_out = msg_out + a[fnd_act]["messages"]["Main"]["msg"][0].get<string>();
                tts_out = tts_out + a[fnd_act]["messages"]["Main"]["tts"][0].get<string>();
                msg_out = msg_out + to_string(price);
                tts_out = tts_out + to_string(price);
            }
            else {
                msg_out = msg_out + a[fnd_act]["messages"]["Fail"]["msg"][0].get<string>();
                tts_out = tts_out + a[fnd_act]["messages"]["Fail"]["tts"][0].get<string>();
            }
        }
        else if (exists(com, "add_good")) {
            string new_good = body["request"]["nlu"]["tokens"][CntToken(fnd_word)];
            int price = 0;
            for (unsigned i = 0; i < body["request"]["nlu"]["entities"].size(); i++) {
                if (body["request"]["nlu"]["entities"][i]["type"] != "YANDEX.NUMBER") continue;
                price = body["request"]["nlu"]["entities"][i]["value"];
                break;
            }
            msg_picker = "Fail";
            if (price != 0) {
                msg_picker = "Main";
                good["item"] = new_good;
                good["price"] = price;
                ses["basket"]["check"].push_back(good);
            }
            msg_out = msg_out + a[fnd_act]["messages"][msg_picker]["msg"][0].get<string>();
            tts_out = tts_out + a[fnd_act]["messages"][msg_picker]["tts"][0].get<string>();
        }
        else if (exists(com, "dell_all")) {
            ses["basket"] = json::parse(u8R"({ "user_id": "", "check" : [] })");  // очистили корзину
            ses["basket"]["user_id"] = body["session"]["user"]["user_id"];        // запомнили id пользователя
        }
        else if (exists(com, "end session")) { //"send webhooks" "end session"

            if (SendWebhooks(ses["basket"])) {
                msg_out = msg_out + u8"\nError";
                tts_out = tts_out + u8"Отсылка пока не работает";
            }
            else {
                return MakeReply(msg_out, tts_out, true);
            }
        }
    }
    if (msg_out == "") { msg_out = u8"Что-то не так..."; }

    return MakeReply(msg_out, tts_out);
}

void gen_response(const Request& req, Response& res) {

    json body = json::parse(req.body.c_str()); //u8     //cout << req.body.c_str()<<"\n\n";

    json ttt = OrderProcess(body);

    res.set_content(ttt.dump(), "text/json; charset=UTF-8");
}

bool ConfigSave(json& cfg) {
    ofstream cfg_file(WHConfig_filename);
    if (!cfg_file.is_open()) {
        cout << "failed to save config.\n";    // some file create error
        return false;
    }
    cfg_file << cfg; cfg_file.close(); // {"webhooks":["site1.com","site2.com"]}
    cout << "done.\n";
    return true;
}

bool JsonRead(json& json_data, string filename = WHConfig_filename) {
    // Checks Webhooks_Cfg.json, reads it or creates empty one. 
    // Opens Dialog.json and reads it
    std::cout << "Opening file: " << filename << " ... ";
    ifstream json_file(filename);
    if (!json_file.is_open()) {
        std::cout << "not found";
        if (filename == WHConfig_filename) { // no config -> create new -> fill empty
            std::cout << " ... creating new ... "; json_data = json::parse(R"({"webhooks": []})");
            json_file.close();
            return ConfigSave(json_data);
        }
        else {
            std::cout << "!!! Critical error !!! Server Stoped !!!";
            json_file.close();
            return false;
        }
    }

    std::cout << "reading ... ";
    try { json_file >> json_data; } // json: cfg or dialog: found and opened, try to json::parse it
    catch (json::parse_error& e) {
        std::cout << "\nError!" << e.what() << '\n';
        json_file.close();
        return false;
    }
    json_file.close(); std::cout << "done.\n";
    return true; // cout << cfg.dump(4);
}

void gen_response_WHGetPost(const Request& req, Response& res) {
    if (!JsonRead(cfg)) {
        res.set_content(u8"Какие-то проблемы с файлом конфига.", "text/html; charset=UTF-8");
        return;
    }

    if (req.method == "POST") {
        if (req.has_param("del")) {
            auto site_to_del = req.get_param_value("del");
            for (unsigned i = 0; i < cfg["webhooks"].size(); i++)
            {
                cout << "   json(site_to_del)=" << json(site_to_del) << endl;
                cout << "   i = " << i << " " << cfg["webhooks"][i] << endl;

                if (cfg["webhooks"][i] == site_to_del) {
                    cout << "   del> i = " << i << " " << cfg["webhooks"][i] << endl;
                    cfg["webhooks"].erase(i); ConfigSave(cfg);
                    break;
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

    for (unsigned i = 0; i < cfg["webhooks"].size(); i++) {
        reply_insert.append(del1).append(cfg["webhooks"][i]).append(del2).append(cfg["webhooks"][i]).append(del3);
    } //for (int i = 0; i < stro.length(); i++){if (stro[i] == '\\'){stro.erase(i);}}

    WH_template_text.replace(WH_template_text.find(WH_pattern), size(WH_pattern), reply_insert);
    res.set_content(WH_template_text, "text/html; charset=UTF-8");  // ответ на Get запрос
}

int main() {
    setlocale(LC_ALL, "Russian");
    json dialog;
    if (!JsonRead(dialog, Dialog_filename)) {
        cout << "\n\n>!!! Error !!! Dialog.json file was not detected or broken. Quit. !!!";
        return 0;
    }
    else {
        if (!dialog.contains("/dialog"_json_pointer) ||
            !dialog.contains("/dialog/actions"_json_pointer) ||
            !dialog.contains("/dialog/screens"_json_pointer)) {
            cout << "\n\n>!!! Error !!! Dialog.json file was not detected or broken. Quit. !!!";
            return 0;
        }
        cout << "   > Dialog.json structure OK.\n";
    }

    s = dialog["dialog"]["screens"];  a = dialog["dialog"]["actions"];
    if (exists(dialog["dialog"], "button_exception")) button_exception = dialog["dialog"]["button_exception"];
    dialog.clear();
    JsonRead(cfg);

    Server svr;
    svr.Post("/", gen_response);
    svr.Get("/webhooks", gen_response_WHGetPost);
    svr.Post("/webhooks", gen_response_WHGetPost);
    std::cout << "Server started at: localhost:3000 ... OK\n";
    svr.listen("localhost", 3000);
}