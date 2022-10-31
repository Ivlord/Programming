#include "Auth_Server.h"

Tokens TOK;
unsigned long TO = millis()+2000;

AsyncWebServer server(80); // запускаем на 80 порту асинхронный WebServer

String processor(const String& var){
  if(var == "BUTTONPLACEHOLDER"){
    String buttons ="";
    if (TOK.last_idx == -1 || TOK.UserOK[TOK.last_idx].role == -1) {return "";}
    int role = TOK.UserOK[TOK.last_idx].role;
    for (int led = 0; led < MaxLedsInSystem; led++) {
      buttons += TOK.leds[led].LedButtonText(ROLES[role].led_access[led]);
    }
    return buttons;
  }
  if (var == "LOGINRESULT") {
    if      (TOK.error == 1) { return "Ошибочные Логин:Пароль. Попробуйте ещё раз."; }
    else if (TOK.error == 2) { return "Слишком много пользователей. Попробуйте ещё раз немного позже."; }
    else if (TOK.error == 3) { return "Таймаут сессии. Войдите в систему ещё раз."; }
    else if (TOK.error == 4) { return "Пользователь с таким логином уже пользуется системой."; }
  }
  if (var=="TOKEN"){if (TOK.last_idx!=-1) {return "token="+String(TOK.UserOK[TOK.last_idx].token);} }  
  return String();
}

void setup(){
  TOK.Clr();
  Serial.begin(115200);
  WiFi.begin(ssid, password);
  while (WiFi.status() != WL_CONNECTED){
    Serial.println("Connecting to WiFi..");
    delay(1000);}
  Serial.print("ESP IP: ");    Serial.print(WiFi.localIP()); 
  Serial.print(", Gateway: "); Serial.println(WiFi.gatewayIP());

//if(WiFi.status() == WL_CONNECTED){ 
//  if (MDNS.begin("esp8266")) {  //Start mDNS with name esp8266       
//    Serial.println("MDNS started at esp8266.local");    }  }
  
  TOK.InitLed(WiFi.localIP());
  server.on("/logout", HTTP_GET, [](AsyncWebServerRequest *request){ // выход из системы (отлогин). 
    // переход на авторизацию из JS: function logoutButton() -> setTimeout(function(){ location.href = '/'; }, 500)
    if (request->hasParam(PARAM_TOKEN)) {
      TOK.ClearToken(request->getParam(PARAM_TOKEN)->value().c_str());
      request->send(200, "text/html", "");    }    });

  // root / Login
  server.on("/", HTTP_GET, [](AsyncWebServerRequest *request){ TOK.error = 0; request->send_P(200, "text/html", login_html, processor);    });

  // запрос от пользователя о состоянии светодиодов.
  server.on("/check", HTTP_GET, [](AsyncWebServerRequest *request){ // remake for 3 Led
      Serial.println("\n\n----------------------------------------\n/check");
      if (request->hasParam(PARAM_TOKEN)) {  // "token"
        String tokenGot = request->getParam(PARAM_TOKEN)->value();
        int role = TOK.ReturnAccessCode(tokenGot.c_str());
        Serial.print("has token: "); Serial.println(tokenGot); Serial.print("role: "); 
        Serial.print(role); Serial.print(", last_idx: "); Serial.println(TOK.last_idx);
        
        if (TOK.last_idx == -1){ TOK.error = 3;
              request->send(200, "text/plain", "[]");
              Serial.println("Expired or not found.");
              return;} // токен не найден или устарел
        
        TOK.dump(TOK.last_idx);
        String rep = "[", zpt = "";
        for (int led = 0; led < MaxLedsInSystem; led++) {
          String LFI = TOK.leds[led].LedFullInfo(ROLES[role].led_access[led]);
          if (LFI == "") continue;
          rep += zpt + LFI; zpt=","; } rep += "]";
        Serial.print("   sending: "); Serial.println(rep);
        request->send(200, "application/json", rep); 
        return;
      } else { request->send(200, "text/plain", "[]");
              Serial.println("запрос не содержит токена.");
              } // реквест вообще не содержит токена
  });

  // Ввод пароля
  server.on("/", HTTP_POST, [](AsyncWebServerRequest *request){
    String loginn; String passs;
    int params = request->params();
    for (int i = 0; i < params; i++) {
        AsyncWebParameter *p = request->getParam(i);
        if (p->name()==PARAM_LOGIN){ loginn = p->value().c_str(); }
        if (p->name()==PARAM_PASS) { passs  = p->value().c_str(); }
        }
    // Имеются вырезанные логин и пароль. Или их нет.
    int res = TOK.AddToken(loginn.c_str(), passs.c_str(), request->client()->remoteIP());
    if(res == -1) { request->send_P(200, "text/html", login_html, processor); }
    else { request->send_P(200, "text/html", index_html, processor); TOK.dump(res);}
    });

// =============================== Update =====================
  server.on("/update", HTTP_GET, [] (AsyncWebServerRequest *request) {
    if (request->hasParam(PARAM_TOKEN)) {                                 // "token"
      String tokenGot = request->getParam(PARAM_TOKEN)->value();
      int role = TOK.ReturnAccessCode(tokenGot.c_str());
      Serial.print("\n\n/update token: "); Serial.println(tokenGot);
      if (TOK.last_idx == -1){ TOK.error = 3;
                               request->send(200, "text/plain", "FAIL");
                               Serial.print(" !ERR: failed to find token ");
                               return;} // токен не найден или устарел
      
      Serial.print("    role: "); Serial.print(ROLES[role].login); Serial.print("#"); Serial.print(role);
      if (request->hasParam(PARAM_ELE) && request->hasParam(PARAM_STATE)) {
        int ele   = (request->getParam(PARAM_ELE  )->value()).toInt();
        int state = (request->getParam(PARAM_STATE)->value()).toInt();
        TOK.leds[ele].LedSet(state, request->client()->remoteIP(), ROLES[TOK.UserOK[TOK.last_idx].role].login);
        TOK.UserOK[TOK.last_idx].pressed_ms = millis() + 30000;      
        request->send(200, "text/plain", "OK");
        Serial.print("   ele=");Serial.print(ele); Serial.print( " & state=" ); Serial.print(state);
        Serial.print(", Led "); Serial.print(ele); Serial.print(" установлен: "); Serial.println(state);
        return;
        }
      else{request->send(200, "text/plain", "no action"); return;} // нет данных об изменении состояния сетодиода: игнорим
    }  
  });

    server.begin();
}

void loop(){

  if (millis()>TO){TOK.CheckTokensTO();}
    
  }
