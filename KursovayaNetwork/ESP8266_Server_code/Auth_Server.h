#ifdef ESP32
  #include <WiFi.h>
  #include <AsyncTCP.h>
#else
  #include <ESP8266WiFi.h>
  #include <ESPAsyncTCP.h>
#endif
#include <ESPAsyncWebServer.h>

#include <ESP8266mDNS.h>


#include "LoginPage.h"
#include "MainPage.h"

//
// Настройки системы
//
#define SCLK 14 // clk 74hc595
#define MOSI 13
#define CS   15
#define HC595_MODE true             // управление пинами через 74hc595: только 3 бита(012) - 3 выхода
#define HC595_ShowRoles true        // показывать законнекченных пользователей по ролям (34567) - 5 пользователей MAX

#define UserOKLen         3         // Максимальное количество одновременно законнекченных пользователей
#define OneTokenLen       8         // Количество символов временного ключа доступа
#define TokenTimeout      30000     // Время активности токена (сколько времени не было действий)
#define MaxUsersInSystem  5         // Количество ролей ползователей в системе
#define MaxLedsInSystem   3         // Количество элементов управления

struct user_types {const char* login; const char* password; const int led_access[MaxLedsInSystem];};
const char     nodata[]  = "сервер"; IPAddress no_ip(0, 0, 0, 0);

// логин:пароль WiFi
const char* ssid = "virus5"; const char* password = "rosehotel5";
const char* LEDS[] = {"Музыка в спальне", "Свет в кухне", "Котел"};

// Список ролей (уровней доступа пользователей)
// legend: 0b11 - два бита на один светодиод: 1x - видит, x1-получает информацию
user_types ROLES[] = {{"mama",  "mama",  {0b11, 0b10, 0b11}}, 
                      {"dasha", "dasha", {0b11, 0b11, 0b10}},
                      {"user",  "12345", {0b10, 0b00, 0b00}},
                      {"chief", "admin", {0b11, 0b10, 0b00}},                       
                      {"admin", "admin", {0b11, 0b11, 0b11}},                           
                      };

const char alphabet[] = "0123456789ABCDEFGHIJKLMNOPQRSTUVWXYZabcdefghijklmnopqrstuvwxyz"; 
const int  alphabetLen  = sizeof(alphabet)/sizeof(alphabet[0]);

class cmpChar{
  public:
  static bool c(const char* a, const char* b){ // перегрузка 1
    // проверяет строки. (const char*, X)
    // X={const char, const char*, char var[], string, string&, "string"}
    int i = 0;
    while (i < 50) {                // 50 - макс длина сравниваемых строк
        if (a[i] != b[i]) break;
        if (!a[i++]) return true; // если найден конец двух строк
    } return false; }
  static bool c(const char* a, const char b) { return cmpChar::c(a, &b); }
  static bool c(const char* a, String& b)    { return cmpChar::c(a, b.c_str()); }
  static bool c(const char* a, String* b)    { String tmp = *b; return cmpChar::c(a, tmp.c_str()); }
  static String IP2String(IPAddress ip){ String res="";
    for (int i=0; i<4; i++){res += String(ip[i])+((i==3)?"":".");}; return res; }
  static String Mills2HHMMSS(unsigned long ms) {
    String hh="", mm="", ss="";
    ms = int(ms / 1000);
    ss = String(ms % 60);
    ms = int(ms / 60);
    mm = String(ms % 60);
    hh = String(int(ms / 60));
    if (hh.length()<2) {hh= "0"+hh;}
    if (mm.length()<2) {mm= "0"+mm;}
    if (ss.length()<2) {ss= "0"+ss;}
    return hh + ":" + mm + ":" + ss;
    }
};

class HC595{ uint8_t mosi; uint8_t cs; uint8_t clk;
  public:  // отправка байта в регистр
    HC595(uint8_t _mosi, uint8_t _cs, uint8_t _clk){ // init
      pinMode(_mosi, OUTPUT); 
      pinMode(_cs, OUTPUT); 
      pinMode(_clk, OUTPUT); 
      mosi = _mosi; cs = _cs; clk = _clk; 
      return; Send(0);}
    
    void Send(uint8_t send_byte){
      digitalWrite(cs, LOW); 
        shiftOut(mosi, clk, MSBFIRST, send_byte); 
      digitalWrite(cs, HIGH); delay(1); }  };

HC595 HC = HC595(5,4,0);
// bits012:led012 (LEDS), connected: bits34567 - ROLES01234
uint8_t OUT = B00000000;

class Led{
  public:
    static int led_counter;
    int idx;
    const char* Name = "";

    IPAddress ip;                   // последний ip, который менял этот выход
    const char* login = nodata;     // последний login, который менял этот пин
    unsigned long last_changed = 0; // время последнего изменения
    uint8_t mask = 0;

         Led(){idx=Led::led_counter; Led::led_counter++; mask = 1<<(idx); Name = LEDS[idx];};
    
  void   LedSet(int val, IPAddress _ip = no_ip, const char* _login = nodata){
    ip = _ip; login = _login; last_changed = millis();
    if (val){ OUT|= mask;}    // включаем
    else {OUT&= (~mask); }    // выключаем
    HC.Send(OUT);
    }

  int    LedGet(){return ((OUT&mask)?1:0);} // возвращает текущее значение бита

  String LedStringStatus(){ return ((LedGet())? "checked":""); }

  String LedFullInfo(int accessLVL){    // ==> [idx, LedIntStatus, LedInfo]
    if (!(2 & accessLVL)) {return "";}
    String _LedFullInfo = "[" + String(idx) + "," + String(LedGet()) + "," + LedInfo(accessLVL) + "]";
    return _LedFullInfo;
    }

  String LedInfo(int accessLVL) {          // "admin ip: 111.222.11.1" или " - "
    if (!(1 & accessLVL)) return "\" - \"";    
    String res = "\"" + cmpChar::Mills2HHMMSS(millis() - last_changed);
    res+= "<br>" + String(login) + " (" + cmpChar::IP2String(ip) + ")\"";
    return res;
    }

  String LedButtonText(int accessLVL){
    if (!(2 & accessLVL)) {return "";}
    String idxS = String(idx);
    String but = "<p>" + String(Name) + " (Led-" + idxS + ")" + " <span id=\"state" + idxS + "\"> : " + LedInfo(accessLVL) + "</span></p>\n";
    but += "<p><label class=\"switch\"><input type=\"checkbox\" onchange=\"toggleCheckbox(this)\" id=\"ch"+ idxS +"\"";
    but += LedStringStatus() + "><span class=\"slider\"></span></label></p>\n";
    return but; }
}; 
int Led::led_counter = 0;

class UserAuthPassed {
  public:
    int             role;     // индекс в базе пользователей
    IPAddress       ip;
    char            token[OneTokenLen+1];
    unsigned long   pressed_ms = millis() + 30000; // timeout 30 sec
    
    UserAuthPassed(){role=-1; token[OneTokenLen]=0;}

    void UserLedOff(){ 
      if ((role>MaxUsersInSystem-1) || (role<0)) {return;}
      OUT&= (~(1<<(role+3))); HC.Send(OUT);}

    unsigned long TokenTimeLeft(){ 
      if ((millis() < this->pressed_ms) && (role != -1)) {return (this->pressed_ms - millis());}
      UserLedOff();
      role = -1; return 0;  }

    String dump(){
      if (role == -1) return "offline";
      String S = "==========\nUser: " + String(ROLES[role].login) + "("+String(role)+")" + cmpChar::IP2String(ip);
      S += "\n   token: " + String(token) + ", время: " + String(TokenTimeLeft()) + "ms"; return S; }      
};

class Tokens{                               // Active Tokens
  public:
    int error = 0;                          // 0-ok, 1-bad pass, 2-NoFree, 3-Expired
    int last_idx = -1;
    Led leds[MaxLedsInSystem];
    UserAuthPassed UserOK[UserOKLen];

  Tokens(){}

  void InitLed(IPAddress _ip){
    for (int i = 0; i < MaxLedsInSystem; i++) {leds[i].ip = _ip;} }

  void Clr(){OUT = 0; HC.Send(0);}

  // добавляет полностью клиента. возвращает его индекс в базе или -1 и error (тип)
  int AddToken(const char * LoginToCompare, const char * PasswordToCompare, IPAddress ip){ // Пробуем добавить Токен
      error = 0;
      int role = -1;      // проверка пароля
      for (int i = 0; i < MaxUsersInSystem; i++) {
        if (cmpChar::c(ROLES[i].login, LoginToCompare) && cmpChar::c(ROLES[i].password, PasswordToCompare)){ 
          role = i; break; } }
      if (role == -1) { error = 1; return -1; } // login:pass - не найдены

      for (int i = 0; i < UserOKLen; i++) {     // проверка не залогинен ли уже такой пользователь
        if (UserOK[i].role == role) { error = 4; return -1; }  }
      
      int free_index = -1;
      for (int i = 0; i < UserOKLen; i++) { if (UserOK[i].role == -1) { free_index = i; break; }  }
      if (free_index == -1) { error = 2; return -1; } // 2 - нет свободных мест для клиентов

      OUT|= (1<<(role+3)); HC.Send(OUT);
      
      GenSetTokenToUser(free_index);
      UserOK[free_index].role       = role;
      UserOK[free_index].ip         = ip;
      last_idx                      = free_index;
      UserOK[free_index].pressed_ms = millis() + TokenTimeout;
      return free_index;
    }

  void GenSetTokenToUser(int cli_idx){// генерирует Токен
      srand(millis()); UserOK[cli_idx].token[OneTokenLen] = 0;
      for (int i = 0; i < OneTokenLen; i++) { UserOK[cli_idx].token[i] = alphabet[ rand() % alphabetLen ]; }  }

  void CheckTokensTO(){
     for (int i = 0; i < UserOKLen; i++) {
      if (UserOK[i].role == -1) continue;
      UserOK[i].TokenTimeLeft(); } }

  int ReturnAccessCode(const char * _token){
    FindToken(_token);
    if (last_idx == -1) return -1; // не найдет токен доступа нет никакого
    return UserOK[last_idx].role;
    }
  
  void FindToken(const char * _token){    // ищет token по базе: -1 -нету, или индекс
    for (int i = 0; i < UserOKLen; i++) { 
      UserOK[i].TokenTimeLeft();
      if (UserOK[i].role == -1) continue;
      if ( cmpChar::c(_token, UserOK[i].token) ) {last_idx = i; return;}
    } last_idx = -1; }

  void ClearToken(const char * _token){   // обнуляет токен после выхода из системы
    FindToken(_token);
    if (last_idx != -1) {
      UserOK[last_idx].pressed_ms = 0;
      UserOK[last_idx].UserLedOff();
      UserOK[last_idx].role == -1;}
    }

  void dump(int idx) {Serial.println(UserOK[idx].dump()+"\n");}
  void dumpAll(){ for (int i=0; i<UserOKLen; i++){dump(i);}}
};

const char* PARAM_STATE = "state";
const char* PARAM_ELE   = "ele";
const char* PARAM_TOKEN = "token";
const char* PARAM_LOGIN = "setl";
const char* PARAM_PASS  = "setp";
