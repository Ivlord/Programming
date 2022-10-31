const char index_html[] PROGMEM = R"rawliteral(
<!DOCTYPE HTML><html>
<head><meta charset='utf-8'>
  <title>Система управления через интернет.</title>
  <meta name="viewport" content="width=device-width, initial-scale=1">
  <style>
    html {font-family: Arial; display: inline-block; text-align: center;}
    h2 {font-size: 2.6rem;}
    body {max-width: 600px; margin:0px auto; padding-bottom: 10px;}
    .switch {position: relative; display: inline-block; width: 120px; height: 68px} 
    .switch input {display: none}
    .slider {position: absolute; top: 0; left: 0; right: 0; bottom: 0; background-color: #ccc; border-radius: 34px}
    .slider:before {position: absolute; content: ""; height: 52px; width: 52px; left: 8px; bottom: 8px; background-color: #fff; -webkit-transition: .4s; transition: .4s; border-radius: 68px}
    input:checked+.slider {background-color: #2196F3}
    input:checked+.slider:before {-webkit-transform: translateX(52px); -ms-transform: translateX(52px); transform: translateX(52px)}
  </style>
</head>
<body>
  <h2>Сервер ESP8266.</h2>
  <h3>Курсовая работа. Сети. Мазлов ИВТ-201(1)</h3>
%BUTTONPLACEHOLDER%
  <button onclick="logoutButton()">Выйти</button>

<script>
function callbackESP(){
    var xhr = new XMLHttpRequest();
    
    xhr.onload = function() {
    if (xhr.status != 200) { // проверка HTTP-статуса ответа, если статус не 200, то - ошибка
      alert(`Ошибка ${xhr.status}: ${xhr.statusText}`); location.href = '/'; } 
    else if (xhr.responseText == "[]") {location.href = '/';} 
    else { const obj = JSON.parse(xhr.responseText);
           for (let i = 0; i < obj.length; i++) {
             document.getElementById("ch" + obj[i][0]).checked = Boolean(obj[i][1]);
             document.getElementById("state"  + obj[i][0]).innerHTML = (obj[i][1] ? "включено " : "выключено " ) + obj[i][2];}
         }  };
    xhr.open("GET", "/check?%TOKEN%", true); xhr.send(); 
  }

let timerId = setTimeout(function tick() { callbackESP(); timerId = setTimeout(tick, 1000); }, 500);

function toggleCheckbox(element) {
  var xhr = new XMLHttpRequest();

  xhr.onload = function() {
    if (xhr.status != 200) {alert(`Ошибка ${xhr.status}: ${xhr.statusText}`); location.href = '/'; } 
    else if (xhr.responseText=='FAIL'){location.href = '/';} };

  let elid = element.id.slice(2);
  let repl = "/update?" + "%TOKEN%" + "&state=" + (element.checked ? "1" : "0") + "&ele=" + elid;
  document.getElementById("state" + elid).innerHTML = "<br> ~ ";
  
  xhr.open("GET", repl, true); xhr.send();
}

function logoutButton() {
  var xhr = new XMLHttpRequest();
  xhr.open("GET", "/logout" + "?%TOKEN%", true); xhr.send();
  setTimeout(function(){ location.href = '/'; }, 200);
}
</script>
</body>
</html>
)rawliteral";
