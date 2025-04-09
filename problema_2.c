/**
Varianta 17 - Problema 2
3.	Care este comanda care porneste un server TCP netcat pe adresa locala si pe portul 8080 ? Mentionati comanda si utilitatea fiecarei optiuni.

nc -l 127.0.0.1 -p 8080

nc – comanda pentru netcat, utilitar de retea, permite citirea si scrierea datelor prin conexiuni TCP sau UDP

-l – din listen, indica faptul ca netcat trebuie sa asculte pentru conexiuni (adica sa functioneze ca un server).

127.0.0.1 – adresa IP locala (localhost). Serverul va accepta conexiuni doar de la acelasi calculator.

-p 8080 – specifica portul 8080 pe care serverul va asculta.
*/
