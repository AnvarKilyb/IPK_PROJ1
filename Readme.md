# Implementace serveru v jazyce C++ komunikujícího prostřednictvím protokolu HTTP

Projekt do předmetu IPK/2022

Autor : Anvar Kilybayev (xkilyb00@stud.fit.vutbr.cz)

## Úvod

 Server poskytuje různé informace o systému. Server naslouchá na zadaném portu a podle url vrací požadované informace. 

 Server musí správně zpracovávat hlavičky HTTP a vytvářet správné HTTP odpovědi. Typ odpovědi bude text/plain. Komunikace se serverem by měla být možná jak pomocí webového prohlížeče, tak nástroji typu wget a curl. Server musí být spustitelný v prostředí Linux Ubuntu 20.04 LTS  (https://ubuntu.com/).

 - Using TCP (IPv4, IPv6)
 - C++14
 - Pro implementaci serveru používá knihovnu soketů
 - Pro komunikace se serverem by měla být možná jak pomocí webového prohlížeče, tak nástroji typu curl

## Spuštění projektu

 Makefile vytvoří spustitelný soubor hinfosvc

 Tento server bude spustitelný s argumentem označující lokální port na kterém bude naslouchat požadavkům: ./hinfosvc 12345

 Server bude možné ukončit pomocí CTRL+C. Server bude umět zpracovat následující tři typy dotazů, které jsou na server zaslané příkazem GET:

  - Získání doménového jména
  - Získání informací o CPU 
  - Aktuální zátěž

## Testování 

 Komunikace se serverem jsem testoval pomocí webového prohlížeče a nástroje typu curl
 
