/**
Varianta 17 - Problema 3
3.	Care sunt diferentele cheie intre pthread_create() – pthread_join() si fork() – exec() – wait() ? Exemplu.

pthread_create() creeaza un thread (fir de executie) in acelasi proces, in timp ce fork() creeaza un proces nou (copil), separat de parinte.

Thread-urile impart acelasi spatiu de memorie, pe cand procesele au spatii de memorie separate.

Comunicarea intre thread-uri e directa, prin variabile comune. Intre procese este nevoie de mecanisme speciale (pipe, socket, etc.).

Un thread ruleaza o functie din acelasi program. Dupa fork(), copilul poate executa un alt program cu exec().

pthread_join() asteapta finalizarea unui thread. wait() asteapta terminarea unui proces copil.

Thread-urile sunt mai rapide si consuma mai putine resurse. Procesele sunt mai izolate si mai sigure.


Exemplu thread:

#include <stdio.h>
#include <pthread.h>

void* functie(void* arg) {
    printf("Salut din thread!\n");
    return NULL;
}

int main() {
    pthread_t t;
    pthread_create(&t, NULL, functie, NULL);
    pthread_join(t, NULL);
    printf("Thread terminat.\n");
    return 0;
}

Exemplu proces:

#include <stdio.h>
#include <unistd.h>
#include <sys/wait.h>

int main() {
    pid_t pid = fork();
    if (pid == 0) {
        execlp("echo", "echo", "Salut din proces!", NULL);
    } else {
        wait(NULL);
        printf("Proces copil terminat.\n");
    }
    return 0;
}

*/
