/**
Varianta 9 - Problema 2
2.	Care este diferenta dintre execle si execve ? Exemple.
*/

// Diferenta dintre execle si execve:

// execle:
// Executa un program si inlocuieste procesul curent.
// Sintaxa: execle(path, arg1, ..., NULL, envp[])
// Exemplu:

// char *env[] = { "HOME=/usr/home", NULL };
// execle("/bin/ls", "ls", "-l", NULL, env);


// execve:

// Similar cu execle, dar argumentele sunt date sub forma de tablouri.
// Sintaxa: execve(path, argv[], envp[])
// Exemplu:

// char *argv[] = { "ls", "-l", NULL };
// char *env[] = { "HOME=/usr/home", NULL };
// execve("/bin/ls", argv, env);


// Diferente:
// execle: lista de argumente urmata de variabilele de mediu.
// execve: argumentele si mediul sunt transmise sub forma de tablouri.






