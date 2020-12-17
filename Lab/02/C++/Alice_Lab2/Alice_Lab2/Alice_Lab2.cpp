#include <iostream> //#include <cmath> //#include <iomanip>
#include <cpp_httplib/httplib.h>
#include <nlohmann/json.hpp>
#include <vector>
#include <string>
#include <iostream>
#include <fstream>
#include "Alisa_srv.h"

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

string s_false = u8"false"; string s_true = u8"true";
string s_general[] = { u8R"({ "response": { "text": ")",
                            u8R"(", "tts": ")",
                            u8R"(", "buttons": [ )",
                            u8R"( ], "end_session": )",
                            u8R"(}, "version": "1.0" })" };
string s_button[] = { u8R"({ "title": ")" , u8R"(", "hide": ")" , u8R"(" })" }; //"payload": {},

// Секция предустановок
int Can_talk = 1;  // 0- Ализа заткнулась,  1 - продолжает говорить 
string screen = u8"Main";  // 0-добавление в корзину ,  1- режим помощи
string user_id, session_id;

json cfg, dialog; // config file and dialog file
json b = {}, s = {}, a = {}; // short names for dialog sections: buttons, screens, actions
json basket = json::parse(u8R"({ "user_id": "", "check" : [] })");
json good = json::parse(u8R"({ "item": "", "price" : 0 })");

void ppp(json& data, bool yy = true) {
    ofstream logger("log.txt");
    logger << "\n" << data.dump(1) << "\n==================================================================\n\n\n";
    if (yy) logger.close();
}

bool exists(const json& j, const string& key) { // проверка, есть ли ключ в словаре
    return j.find(key) != j.end();
}

string MakeBtn(json& button_list, string button_name, string s_plus = "") {
    string hide = "";
    int hide_can_talk;
    for (unsigned i = 0; i < button_list.size(); i++) {
        if (button_list[i]["Title"] != button_name) continue;
        hide = button_list[i]["hide"];
        hide_can_talk = button_list[i]["Hide Can_talk="].get<int>();
        break;
    }

    if (hide == "" || hide_can_talk == Can_talk) return "";
    return s_button[0] + button_name + s_button[1] + hide + s_button[2] + s_plus;
}

string MakeBtns(json& button_list, json& need_button_list, string s_plus = "") {
    string res = "";
    string tmp = "";
    bool first_run = true;

    for (unsigned i = 0; i < need_button_list.size(); i++) {
        tmp = MakeBtn(button_list, need_button_list[i].get<string>());
        if (tmp == "") continue;

        res = res + ((first_run) ? "" : ", ") + tmp;
        first_run = false;
    }
    return res + s_plus;
}

string MakeReply(string txt, string tts, json& button_list, json& need_button_list, string end_session = "false") {
    //cout << "   >MakeReply \n   " << button_list.dump(4) << "\n" << need_button_list.dump(4);
    string res = s_general[0] + txt + s_general[1] + tts + s_general[2];
    res = res + MakeBtns(button_list, need_button_list, "");
    res = res + s_general[3] + end_session + s_general[4];
    return res;
}

bool In_str(string main_string, string sub_string) {
    // возвращает true, если подстрока найдена в строке
    return (main_string.find(sub_string) != std::string::npos) ? true : false;
}

int CntToken(string data) {
    int cnt = 1; // points to the next token
    while (data.find(" ") != std::string::npos) {
        cnt += 1;
        data.replace(data.find(" "), size(" "), "");
    }
    return cnt;
}

//"tts": "sil <[500]>" //["session"]["user"]["user_id"] or "anonymous"
string OrderProcess(json& body) {
    string msg_out = "", tts_out = "";
    json a_need = s[screen]["actions"];       // actions for this screen - list
    json fnd_act = {}; string fnd_word = "";  // для совпавшей action и конкретно совпавшего слова
    string com_val_str = "";                  // правильное сообщение

    for (auto& act : a) { //a[name]["check"] - list  //a[name]["react"] - {}
        ppp(act, 0);
        for (auto& check : act["check"]) {
            if (!In_str(body["request"]["original_utterance"].get<string>(), check)) continue;// нет совпадения - дальше
            fnd_act = act;    // запоминаем "спаботавший" action
            fnd_word = check;    // и конкретное слово, на котором сработало
            break;
        }
        if (!fnd_act.is_null()) break;
    }
    ppp(fnd_act);

    //logger <<"\n\n"<< fnd_act.dump(3) << "\n\n==================================================================\n\n\n";

    if (fnd_act.is_null()) { // нихера не нашли, возвращаем "я не знаю такой команды" // доделать
        msg_out = u8"Команда не распознана";// .get<string>();
        tts_out = u8"Команда не распознана";

        return MakeReply(msg_out, tts_out, b, s[screen]["buttons"], "false");
    }

    // command processing section
    for (auto& com : fnd_act["react"][screen]) { // com= [{"CgangeVar":"Can_talk", "Val":false}, {"Msg":"Start"}]
        if (exists(com, "Msg")) {              // обработка команд Msg: устанавливаем текст ответа
            com_val_str = com["Msg"];
            msg_out = msg_out + fnd_act["messages"][com_val_str]["msg"][0].get<string>();
            tts_out = tts_out + fnd_act["messages"][com_val_str]["tts"][0].get<string>();
            continue;
        }
        else if (exists(com, "ChangeVar_Can_talk")) {
            Can_talk = com["ChangeVar_Can_talk"].get<int>();
        }
        else if (exists(com, "ChangeVar_screen")) {
            screen = com["ChangeVar_screen"].get<string>();
        }
        else if (exists(com, "print")) {} // так и надо
        else if (exists(com, "del_good")) { //"del_good""add_good""sum""dell_all""list"
            string new_good = body["request"]["nlu"]["tokens"][CntToken(fnd_word)];
            bool fnd_good = false;
            for (unsigned i = 0; i < basket["check"].size(); i++) {
                if (basket["check"][i]["item"] != new_good) continue;
                basket["check"].erase(i);
                fnd_good = true;
                break;
            }
            com_val_str = "Fail";
            if (fnd_good) com_val_str = "Main";
            msg_out = msg_out + fnd_act["messages"][com_val_str]["msg"][0].get<string>();
            tts_out = tts_out + fnd_act["messages"][com_val_str]["tts"][0].get<string>();
        }
        else if (exists(com, "list")) { // List of goods
            string all_goods_list = "";

            int cnt = 0;
            bool first_run = true;
            for (unsigned i = 0; i < basket["check"].size(); i++) {
                if (!first_run) all_goods_list.append(u8"\n ");
                all_goods_list.append(basket["check"][i]["item"].get<string>());
                all_goods_list.append("\t");
                all_goods_list.append(basket["check"][i]["price"].get<string>());
                cnt += 1;
                bool first_run = false;
            }

            if (cnt > 0) { // если что-то найдено в корзине
                msg_out.append(fnd_act["messages"]["Main"]["msg"][0].get<string>()).append(u8" ");
                tts_out.append(fnd_act["messages"]["Main"]["tts"][0].get<string>());
                string(msg_out.append(all_goods_list));
                string(tts_out.append(all_goods_list));
            }
            else {
                msg_out = string(msg_out + fnd_act["messages"]["Fail"]["msg"][0].get<string>());
                tts_out = string(tts_out + fnd_act["messages"]["Fail"]["tts"][0].get<string>());
            }
        }
        else if (exists(com, "sum")) { // Sum
            int price = 0;
            for (unsigned i = 0; i < basket["check"].size(); i++) {
                price = price + basket["check"][i]["price"].get<int>();
            }

            if (price != 0) { // есть покупки
                msg_out = msg_out + fnd_act["messages"]["Main"]["msg"][0].get<string>();
                tts_out = tts_out + fnd_act["messages"]["Main"]["tts"][0].get<string>();
                msg_out = msg_out + to_string(price);
                tts_out = tts_out + to_string(price);
            }
            else {
                msg_out = msg_out + fnd_act["messages"]["Fail"]["msg"][0].get<string>();
                tts_out = tts_out + fnd_act["messages"]["Fail"]["tts"][0].get<string>();
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
            com_val_str = "Fail";
            if (price != 0) {
                com_val_str = "Main";
                good["item"] = new_good;
                good["price"] = price;
                basket["check"].push_back(good);
            }
            msg_out = msg_out + fnd_act["messages"][com_val_str]["msg"][0].get<string>();
            tts_out = tts_out + fnd_act["messages"][com_val_str]["tts"][0].get<string>();
        }
        else if (exists(com, "dell_all")) {
            basket = json::parse(u8R"({ "user_id": "", "check" : [] })");  // очистили корзину
            basket["user_id"] = body["session"]["user"]["user_id"];        // запомнили id пользователя
        }
        else if (exists(com, "cmd")) { //"send webhooks" "end session"
            msg_out = msg_out + u8"Отсылка пока не работает";
            tts_out = tts_out + u8"Отсылка пока не работает";
        }
    }
    return MakeReply(msg_out, tts_out, b, s[screen]["buttons"], "false");
}

void gen_response(const Request& req, Response& res) {

    json body = json::parse(req.body.c_str());

    string str = OrderProcess(body);

    res.set_content(str, "text/json; charset=UTF-8");
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
            !dialog.contains("/dialog/screens"_json_pointer) ||
            !dialog.contains("/dialog/buttons"_json_pointer)) {
            cout << "\n\n>!!! Error !!! Dialog.json file was not detected or broken. Quit. !!!";
            return 0;
        }
        cout << "   > Dialog.json structure OK.\n";
    }

    b = dialog["dialog"]["buttons"]; s = dialog["dialog"]["screens"];  a = dialog["dialog"]["actions"];
    dialog.clear();

    JsonRead(cfg);


    //string bbb = MakeBtns(d["dialog"]["buttons"], d["dialog"]["Screens"]["Help"]["buttons"], "");
    //cout << exists(d["dialog"]["Screens"], "Start-") << "\n";
    //cout << exists(d["tst"], u8"yes") << "\n";
    //return 0;

    //bool tst = cfg["dialog"][0]["actions"][0]["is_btn"].get<bool>();  //.get<bool>()
    //if (tst) cout << "True selected" << "\n";
    //else cout << "False selected" << "\n";

    //cout << cfg["dialog"][0]["actions"][0]["check"][0]["is_btn"].get<bool>() << "\n";
    /*cout << m << "\n";
    cout << u8"Молчать" << "\n";
    cout << "Молчать" << "\n";*/

    Server svr;
    svr.Post("/", gen_response);
    svr.Get("/webhooks", gen_response_WHGetPost);
    svr.Post("/webhooks", gen_response_WHGetPost);
    std::cout << "Server started at: localhost:3000 ... OK\n";
    svr.listen("localhost", 3000);
}

/*
//    transform(sl.begin(), sl.end(), sl.begin(), ::tolower);
int Test_py() { // при недоделанной Алисе протестировать пайтон-сервер
    string test = u8R"({ "user_id": "9359F683B13A18A1",
                        "check":[
                                    { "item": "сандальки", "price" : 250 },
                                    { "item": "сандальк1", "price" : 350 },
                                    { "item": "сандальк2", "price" : 450 },
                                    { "item": "сандальк3", "price" : 550 },
                                    { "item": "сандальк4", "price" : 650 },
                                    { "item": "сандальк5", "price" : 750 },
                                    { "item": "сандальк6", "price" : 850 }] } )";
    char ip[] = "http://127.0.0.1:5000";
    char dir[] = "/";

    //.c_str()
    json snd = json::parse(test);
    cout << snd.dump(4);
    Client cli(ip);
    auto resd = cli.Post(dir, snd.dump(), "application/json");

    return 0;

    //json snd = json::object(); // array(); //object();
    //snd.push_back({ "user_id", "9359F683B13A18A1" });
    //snd.push_back({ "user_id", "9359F683B13A18A1" });
    //json ar= json::array();
    //ar.push_back()
}


ofstream logger("log.txt");
string str = "";
MakeReply(str);
logger << str << "\n\n==================================================================\n\n";
return 0;


#include <unicode/unistr.h>
#include <unicode/ustream.h>
#include <unicode/locid.h>

#include <iostream>

int main()
{

char const* someString = u8"ΟΔΥΣΣΕΥΣ";
icu::UnicodeString someUString(someString, "UTF-8");
// Setting the locale explicitly here for completeness.
// Usually you would use the user-specified system locale,
// which *does* make a difference (see ı vs. i above).
std::cout << someUString.toLower("el_GR") << "\n";
std::cout << someUString.toUpper("el_GR") << "\n";
return 0;
}

 //u8     //cout << req.body.c_str()<<"\n\n";

*/

/* string str = u8R"(
 {
   "response": {
     "text": "Здравствуйте! Я помогу вам с покупками.",
     "tts": "Здравствуйте! Я помогу вам с покупками.",
     "buttons": [
         {
             "title": "Помощь",
             "hide": true
         },
         {
             "title": "Молчать",
             "hide": true
         }
     ],
     "end_session": false
   },
   "version": "1.0"
 })";*/