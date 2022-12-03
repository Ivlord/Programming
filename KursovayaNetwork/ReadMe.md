## Курсовая работа по предмету "Сети". 2021.
<h2>"СИСТЕМА УПРАВЛЕНИЯ УМНЫМ ДОМОМ ПО WI-FI  С ПОМОЩЬЮ ESP8266"</h2>
Author: Ivan Mazlov
<ul>

<li>Релизован механизм дистанционного управления условными электропиборами (физическое устройство):</li>
  <ul>
    <li>ESP8266 - основа устройства</li>
    <li>Возможность подключения к серверу по локальной сети (или через NAT)</li>
    <li>Использование 74hc595 индикации подключенных пользователей (5 светодиодов) и статуса устройства (3 светодиода)</li>
  </ul>
<li>Система аутентификации пользователей (c++):
  <ul>
    <li>Уровень доступа пользователя определяется login:password:
      <ul>
        <li>видеть статус устройста</li>
        <li>управлять устройством</li>
        <li>полная сводка</li>
      </ul>
    </li>
    <li>Уникальные логины и мультилогины</li>
    <li>Использование html и Javascript на стороне пользователя</li>
  </ul>
</li>
</ul>