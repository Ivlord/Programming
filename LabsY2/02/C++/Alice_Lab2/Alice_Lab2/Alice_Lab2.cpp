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
json j_shablon_rep = json::parse(u8R"({ "response": { "text": " ", 
                                                      "tts": " ", 
                                                      "buttons": [],
                                                      "end_session": false }, 
                                        "version": "1.0"})");
// "silent_mode"    - 0- Ализа заткнулась,  1 - продолжает говорить 
// "screen":"Main"  - текущий экран
json ses_shablon = json::parse(u8R"({"screen":"Main", "silent_mode":1, "rnd_on": true ,
                                     "session_id":"", "basket": { "user_id": "", "check" : [] } } )");
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
            if (exists(button_exception[new_button_name], ses["screen"])) {
                if (button_exception[new_button_name][(string)ses["screen"]]["silent_mode"] == ses["silent_mode"]) continue;
            }
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
    return reply;
}

// обрезает первый и последние пробелы в строке, удаляет все двойные пробелы
string CutSpaces(string str) {
    size_t index = string::npos;
    while ((index = str.find("  ")) != string::npos)     // удаляем двойные пробелы
        str.erase(index, 1);
    if (str[str.size() - 1] == ' ')  str.erase(str.size() - 1, 1);  // если пробел в конце строки, удаляем
    return str;
}

string Good_Del(string main, string com, bool single = true) {
    if ((main == CutSpaces(com)) || (size(main) < size(com))) return ""; // просто ввели одну команду. "хвоста нет"
    string hvost = CutSpaces(main.substr(size(com)));
    if (!single) {
        if (hvost[0] == ' ') hvost.erase(0, 1); // между командой и товаром есть пробел? отрезаем его
        else  return "";                        // ввели без пробела. ошибка
    }

    return hvost; // возвращаем  "хвост" (название товара)
}

string Good_Add(string main, string com) {
    string hvost = Good_Del(main, com, false); // отрезаем команду в начале строки
    if (empty(hvost)) return ""; // хвост пустой? ошибка

    // ищем цену - первая цифра должна быть
    size_t first_digit_index = hvost.find_first_of("0123456789");

    // не нашли или нет одного или двух необходимых пробелов(даже символов для них): в начале и перед ценой
    //             не нашли                     нет текста товара             нет пробела перед цифрой
    if ((first_digit_index == string::npos) || (first_digit_index == 0) || (hvost[first_digit_index - 1] != ' ')) {
        return "";
    }
    return CutSpaces(hvost.substr(0, first_digit_index)); // вырезаем всё до цифры, убираем пробелы
}

bool SendWebhooks(json& basket, string site) {
    string url;
    string dir;
    size_t index = site.find("//");
    if (index == string::npos) {  // сайт введен без https://
        index = site.find("/");
        index = 0;
    }
    else { index += 2; }

    index = site.find("/", index);

    if (index == string::npos) {     // без dir
        url = site; dir = string("/");
    }
    else {
        url = site.substr(0, index);
        dir = site.substr(index);
    }

    cout << "   > url: " << url << " dir: " << dir << " ... ";

    Client cli(url.c_str());
    auto cli_res = cli.Post(dir.c_str(), basket.dump(), "application/json");

    if (cli_res) { // Проверяем статус ответа, т.к. может быть 404 и другие
        if (cli_res->status == 200) {
            cout << " ok\n";   // В res->body лежит string с ответом сервера
            return true;
        }
        else  std::cout << "Status code: " << cli_res->status << std::endl;
    }
    else { auto err = cli_res.error();  std::cout << "Error code: " << err << std::endl; }
    return false;
}

void fill_msg(string& msg, string& tts, string& fnd_act, string msg_picker, json& a) {
    int msg_idx = (ses["rnd_on"]) ? rand() % a[fnd_act]["messages"][msg_picker]["msg"].size() : 0;
    msg += a[fnd_act]["messages"][msg_picker]["msg"][msg_idx].get<string>();
    tts += a[fnd_act]["messages"][msg_picker]["tts"][msg_idx].get<string>();
}

json OrderProcess(json& body) {
    srand(time(NULL));

    string msg_picker = "";                   // ключ правильного сообщения
    int msg_idx = 0;
    // Инициализация новой сессии с приветственным ответом. +обнуление корзины.
    if (body["session"]["new"].get<bool>()) { // true-для новой сесии, false- продолжение
        ses = ses_shablon;
        ses["session_id"] = body["session"]["session_id"];                        // сохраняем новый id сессии

        if (empty(body["session"]["user"]["user_id"])) ses["basket"]["user_id"] = "anonymous";
        else ses["basket"]["user_id"] = body["session"]["user"]["user_id"];  // и id пользователя или "anonymous"

        cout << " > new session:" << ses["session_id"] << "\n";
        cout << " > user_id:" << ses["basket"]["user_id"] << "\n";

        msg_picker = ses["screen"];
        return MakeReply(s[msg_picker]["in_msg"][0], s[msg_picker]["in_tts"][0]);
    }
    else if (body["session"]["session_id"] != ses["session_id"]) {    // пришла сессия не с тем id
      // завершаем разговор и валим закрываем сессию=true
        return MakeReply(s["Main"]["out_msg"][0], s["Main"]["out_tts"][0], true);
    }
    string ut = body["request"]["original_utterance"].get<string>();
    string msg_out = "", tts_out = ""; // спул сообщений

    string fnd_word = "", fnd_act = "";  // для совпавшей конкретно совпавшего слова и action 
    string screen = ses["screen"].get<string>();
    string screen_fail_msg = s[screen]["fail_msg"][0].get<string>();
    string screen_fail_tts = s[screen]["fail_tts"][0].get<string>();

    json sa = s[screen]["actions"];

    for (unsigned i = 0; i < sa.size(); i++) {                  // список экшинов на экране
        string new_chck = sa[i].get<string>();                  // конкретный экшин

        if (!exists(a, new_chck)) continue;                          // такой реакции нет в списке реакций. пропускаем.
        for (unsigned j = 0; j < a[new_chck]["check"].size(); j++) { // идём оп словам контрольным

            if (ut.find(a[new_chck]["check"][j].get<string>()))
                continue;                   // нет совпадения так как в нулевом индексе строки не нашли - дальше
            fnd_act = new_chck;                                // запоминаем "сработавший" action
            fnd_word = a[new_chck]["check"][j];                 // и конкретное слово, на котором сработало
            break;
        } if (!empty(fnd_act)) break;
    }

    if (empty(fnd_act)) { // никаких признаков правильного ввода от пользователя, возвращаем "я не знаю такой команды"
        return MakeReply(screen_fail_msg, screen_fail_tts);
    }

    //// command processing section
    for (auto& com : a[fnd_act]["react"][screen]) {
        if (exists(com, "Msg")) {                                // обработка команд Msg: устанавливаем текст ответа
            fill_msg(msg_out, tts_out, fnd_act, com["Msg"], a);  // com["Msg"] - берём название текста, для печати из значения команды "Msg"
            continue;
        }                                          // можно добавлять много текстов подряд

        else if (exists(com, "ChangeVar_Can_talk")) {
            ses["silent_mode"] = com["ChangeVar_Can_talk"].get<int>();
        }

        else if (exists(com, "ChangeVar_screen")) {
            ses["screen"] = com["ChangeVar_screen"].get<string>();
        }

        else if (exists(com, "command_list")) {
            if (screen == "Help") {
                for (unsigned i = 0; i < a[fnd_act]["check"].size(); i++) {
                    msg_out += a[fnd_act]["check"][i].get<string>().append((i == a[fnd_act]["check"].size() - 1) ? "." : ", ");
                    tts_out += a[fnd_act]["check"][i].get<string>().append(",");
                }
                continue;
            }
        }

        else if (exists(com, "del_good")) { // delete good
            string del_good = Good_Del(ut, fnd_word, false);
            if (empty(del_good)) return MakeReply(screen_fail_msg, screen_fail_tts);

            bool fnd_good = false;
            for (unsigned i = 0; i < ses["basket"]["check"].size(); i++) {
                if (ses["basket"]["check"][i]["item"] != del_good) continue;

                ses["basket"]["check"].erase(i);
                fnd_good = true;
                break;
            }
            fill_msg(msg_out, tts_out, fnd_act, (fnd_good) ? "Main" : "Fail", a);
        }

        else if (exists(com, "list")) { // List of goods
            if (!empty(Good_Del(ut, fnd_word))) return MakeReply(screen_fail_msg, screen_fail_tts);
            int cnt = 0;
            bool first_run = true;
            for (unsigned i = 0; i < ses["basket"]["check"].size(); i++) {
                if (first_run) {
                    fill_msg(msg_out, tts_out, fnd_act, "Main", a);
                    first_run = false;
                }
                string m1 = string(ses["basket"]["check"][i]["item"]);
                string m2 = to_string(ses["basket"]["check"][i]["price"]);
                msg_out = msg_out + m1 + "\t" + m2 + "\n";
                tts_out = msg_out + m1 + "\t" + m2 + "\n";
                cnt += 1;
            }
            if (cnt == 0) fill_msg(msg_out, tts_out, fnd_act, "Fail", a); // если не найдено в корзине
        }

        else if (exists(com, "sum")) { // Sum
            if (!empty(Good_Del(ut, fnd_word))) return MakeReply(screen_fail_msg, screen_fail_tts);
            int price = 0;
            for (unsigned i = 0; i < ses["basket"]["check"].size(); i++) {
                price += ses["basket"]["check"][i]["price"].get<int>();
            }

            fill_msg(msg_out, tts_out, fnd_act, (price) ? "Main" : "Fail", a);
            if (price) {                       // есть покупки
                msg_out += to_string(price);   // дописываем сумму
                tts_out += to_string(price);
            }

        }

        else if (exists(com, "add_good")) {
            string new_good = Good_Add(ut, fnd_word);
            if (empty(new_good)) return MakeReply(screen_fail_msg, screen_fail_tts);

            int price = 0;
            int price_end_token = 0;
            for (unsigned i = 0; i < body["request"]["nlu"]["entities"].size(); i++) {
                if (body["request"]["nlu"]["entities"][i]["type"] != "YANDEX.NUMBER") continue;
                price = body["request"]["nlu"]["entities"][i]["value"];
                price_end_token = body["request"]["nlu"]["entities"][i]["tokens"]["end"].get<int>();
                break;
            }

            int tokens_amount = size(body["request"]["nlu"]["tokens"]);
            if (tokens_amount > price_end_token) {// после цены идут ещё токены, проверяем
                string after_price_token = body["request"]["nlu"]["tokens"][price_end_token].get<string>();

                if ((after_price_token != u8"р") && (after_price_token != u8"руб") && (after_price_token != u8"р.")) { // не та валюта. ошибка
                    return MakeReply(screen_fail_msg, screen_fail_tts);
                }
                if (tokens_amount > price_end_token + 1) {// слишком много токенов после цены. ошибка
                    return MakeReply(screen_fail_msg, screen_fail_tts);
                }
            }

            if (price) {
                good["item"] = new_good;              // цена не равна нулю. добавляем товар
                good["price"] = price;
                ses["basket"]["check"].push_back(good);
            }

            fill_msg(msg_out, tts_out, fnd_act, "Main", a);
        }

        else if (exists(com, "dell_all")) { // очистка корзины
            if (!empty(Good_Del(ut, fnd_word))) return MakeReply(screen_fail_msg, screen_fail_tts);

            fill_msg(msg_out, tts_out, fnd_act, (!ses["basket"]["check"].size()) ? "Fail" : "Main", a);

            ses["basket"] = json::parse(u8R"({ "user_id": "", "check" : [] })");  // очистили корзину

            if (empty(body["session"]["user"]["user_id"])) ses["basket"]["user_id"] = "anonymous";
            else ses["basket"]["user_id"] = body["session"]["user"]["user_id"];  // запомнили id пользователя
        }

        else if (exists(com, "end session")) { //"send webhooks" "end session"
            if (!empty(Good_Del(ut, fnd_word))) return MakeReply(screen_fail_msg, screen_fail_tts);

            cout << "Session closed. Sending data to:\n";
            for (int i = 0; i < cfg["webhooks"].size(); i++) {
                string webhook_url = cfg["webhooks"][i].get<string>();
                SendWebhooks(ses["basket"], webhook_url);
            }

            return MakeReply(msg_out, tts_out, true);
        }
    }
    if (msg_out == "") { msg_out = u8"Что-то не так..."; }

    return MakeReply(msg_out, tts_out);
}

void gen_response(const Request& req, Response& res) {

    json body = json::parse(req.body.c_str()); //u8     //cout << req.body.c_str()<<"\n\n";

    res.set_content(OrderProcess(body).dump(), "text/json; charset=UTF-8");
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
            for (unsigned i = 0; i < cfg["webhooks"].size(); i++) {
                if (cfg["webhooks"][i] == site_to_del) {
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
    }

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
