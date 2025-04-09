/**
Varianta 17 - Problema 5

2. Scrieti un script C care creeaza un proces fiu. Procesul parinte va verifica existenta unui director temporar numit "temp_dir".
Daca directorul nu exista, il va crea. Procesul parinte va muta fisierul de intrare specificat pe linia de comanda in acest director.
Procesul fiu va apela, asupra programului head (cu optiunile -n 2), un apel din familia exec*, indicand calea absoluta a fisierului.
Procesul parinte va astepta terminarea procesului fiu si va sterge directorul temporar.
*/

#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <sys/stat.h>
#include <sys/wait.h>
#include <string.h>
#include <errno.h>

#define TEMP_DIR "temp_dir"
#define MAX_PATH 1024

int main(int argc, char* argv[]) {
    if (argc != 2) {
        fprintf(stderr, "Utilizare: %s <fisier_intrare>\n", argv[0]);
        return 1;
    }

    // Verifica daca directorul temp_dir exista; daca nu, il creeaza
    struct stat st;
    if (stat(TEMP_DIR, &st) == -1) {
        if (mkdir(TEMP_DIR, 0700) == -1) {
            perror("mkdir");
            return 1;
        }
    } else if (!S_ISDIR(st.st_mode)) {
        fprintf(stderr, "%s exista dar nu este un director.\n", TEMP_DIR);
        return 1;
    }

    // Mutam fisierul in directorul temp_dir
    char dest_path[MAX_PATH];
    snprintf(dest_path, sizeof(dest_path), "%s/%s", TEMP_DIR, strrchr(argv[1], '/') ? strrchr(argv[1], '/') + 1 : argv[1]);

    if (rename(argv[1], dest_path) == -1) {
        perror("rename (mutare fisier)");
        return 1;
    }

    pid_t pid = fork();
    if (pid == -1) {
        perror("fork");
        return 1;
    }

    if (pid == 0) {
        // Procesul fiu - apelam head -n 2 pe fisierul mutat
        execlp("head", "head", "-n", "2", dest_path, (char*)NULL);
        // Daca exec esueaza:
        perror("execlp");
        exit(1);
    } else {
        // Procesul parinte
        int status;
        waitpid(pid, &status, 0);

        // Stergem fisierul mutat
        if (unlink(dest_path) == -1) {
            perror("unlink (stergere fisier)");
        }

        // Stergem directorul temporar
        if (rmdir(TEMP_DIR) == -1) {
            perror("rmdir");
        }

        if (WIFEXITED(status) && WEXITSTATUS(status) == 0) {
            printf("Executia procesului fiu s-a incheiat cu succes.\n");
        } else {
            fprintf(stderr, "Procesul fiu a esuat sau a fost terminat anormal.\n");
        }
    }

    return 0;
}


/*
1. Verificare si creare director temporar:

    - Se verifica existenta directorului "temp_dir" folosind stat()
    - Daca directorul nu exista, se creeaza cu mkdir() si permisiuni 0700
    - Daca exista dar nu este director, se afiseaza eroare si programul se opreste

2. Mutarea fisierului in directorul temporar:

    - Se construieste calea completa catre noua locatie a fisierului
    - Se muta fisierul cu rename() din locatia initiala in "temp_dir"

3. Procesul fiu:

    - Se creeaza un proces fiu cu fork()
    - In procesul fiu se apeleaza comanda "head -n 2 <fisier>" folosind execlp()
    - Daca exec esueaza, se afiseaza un mesaj de eroare

4. Procesul parinte:

    - Asteapta terminarea procesului fiu cu waitpid()
    - Dupa terminarea procesului fiu, sterge fisierul mutat si apoi directorul temporar
    - Afiseaza un mesaj in functie de codul de iesire al procesului fiu
*/

