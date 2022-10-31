//
// Страница ввода login:password аутентификация пользователя.
//
const char login_html[] PROGMEM = R"rawliteral(
<!DOCTYPE HTML>
  <html>
    <head><title>Login:</title>
    <meta charset='utf-8'><meta name="viewport" content="width=device-width, initial-scale=1">
    <style>
    body{ background: rgb(52, 56, 61);
          padding: 80px; }
    h1{   color: rgb(255, 255, 255);
          width: 200px;
          text-align: left;
          position: fixed;
          margin-top: auto; }
    #wrapper{ position: absolute;
          width: 320px;
          margin-top: 45px; }
    /* === Sign in Form === */
    #signin { height: 90px;
          width: 300px;
          border-radius: 8px;
          position: relative; }
    #signin::before {
          display: block;
          position: relative;
          height: 2px;
          background: rgb(52, 56, 61);
          content: '';
          top: 44px;
          margin-left: 20px;
          z-index: 1; }
    #signin input:first-of-type{
          border-top-right-radius: 8px;
          border-top-left-radius: 8px;}
    #signin input:last-of-type{
          border-bottom-right-radius: 8px;
          border-bottom-left-radius: 8px; }
    #signin  input[type="text"], #signin  input[type="password"], #signin button[type="submit"]{
          background: rgb(28, 30, 33);
          box-shadow: inset -100px -100px 0 rgb(28, 30, 33); /*Prevent yellow autofill color*/
          color: rgb(52, 56, 61); }
    #signin  input[type="text"], #signin  input[type="password"]{
          position: relative;
          display: block;
          width: 280px;
          height: 45px;
          border: 0;
          outline: 0;
          top: -2px;
          padding: 0 0 0 20px;
          font-weight: 700; }
    #signin  input[type="text"]:focus, #signin  input[type="password"]:focus{
          color: rgb(255, 255, 255); }
    #signin button[type="submit"]{
          display: block;
          position: absolute;
          width: 52px;
          height: 52px;
          color: rgb(52, 56, 61);
          border-radius: 50px;
          outline: 0;
          z-index: 2;
          top: 19px;
          right: -24px;
          border: 6px solid rgb(52, 56, 61);
          font-size: 25px;
          text-indent: 0px;
          padding-left: 9px;
          padding-bottom: 3px;
          text-align: center; }
    #signin button[type="submit"]:hover{
          color: rgb(0, 126, 165);
          text-shadow: 0 0 10px rgb(0, 126, 165);
          cursor: pointer; }
    #signin p {
          color: rgb(79, 85, 97);
          padding: 0 20px;
          font-weight: 700;
          font-size: 12px;
          margin: 5px 0 0 0; }
    #signin p > a{
          color: rgb(111, 119, 135);
          text-decoration: none; }
    #signin p > a:hover{
          border-bottom: 1px solid; }
    </style>
    </head>
    <body>
      <h1>Авторизация</h1>
        <div id="wrapper">
        <form id="signin" method="post" action="" autocomplete="off">
          <input type="text" id="user" name="setl" placeholder="Login" />
          <input type="password" id="pass" name="setp" placeholder="Password" />
          <button type="submit"> > </button>
          <p>%LOGINRESULT%</p>
        </form>
        </div>
    </body>
  </html>)rawliteral";
