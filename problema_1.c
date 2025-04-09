/**
Varianta 17 - Problema 1
1.	Cum sunt reprezentate si stocate argumentele de pe linia de comanda ? Dar variabilele de mediu transmise unui program C ? Mentionati valorile argc, argv si envp pentru urmatoarea linie:
export X=2; export Y=3; ./a.out -a 2 -b 3

argc (argument count) – numarul total de argumente, inclusiv numele programului
argv (argument vector) – vector de char*, fiecare element fiind un argument, argv[0] este de obicei calea/numele programului (ex: "./problema1.out").
envp (environment pointer) – vector de char* care contine variabilele de mediu în formatul "NUME=valoare".

Variabilele de mediu sunt stocate in vectorul envp[] (sau accesibile prin functii ca getenv()),
sunt setate inainte de executia programului (in shell, de ex. cu export).

export X=2; export Y=3; ./a.out -a 2 -b 3

X=2 și Y=3 sunt variabile de mediu.

./a.out -a 2 -b 3 este executia programului cu 4 argumente suplimentare.

argv:
argv[0] = "./a.out"
argv[1] = "-a"
argv[2] = "2"
argv[3] = "-b"
argv[4] = "3"
argv[5] = NULL
argc = 5

envp[0] = "X=2"
envp[1] = "Y=3"
urmate de celelalte variabile de mediu din sistem
envp[n] = NULL

*/
