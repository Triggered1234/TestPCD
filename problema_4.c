/**
Varianta 17 - Problema 4

Scrieti un script C care preia ca argument pe linia de comanda un director.
Scriptul va deschide directorul, va traversa si selecta DOAR fisierele regulare din acesta,
iar pentru fiecare din acestea, va crea un link simbolic in /tmp/<nume_fisier>,
unde nume_fisier este numele fisierului original.

Pentru fiecare fisier regular din directorul dat, se va crea un thread separat,
care va fi responsabil de crearea linkului simbolic.
La final, se va astepta terminarea tuturor thread-urilor.
*/

#include <stdio.h>
#include <stdlib.h>
#include <dirent.h>
#include <pthread.h>
#include <sys/stat.h>
#include <unistd.h>
#include <string.h>

#define TMP_DIR "/tmp/"
#define MAX_PATH_LENGTH 1024

// Structura folosita pentru a pasa informatia fiecarui thread
typedef struct {
    char source_path[MAX_PATH_LENGTH];  // calea completa catre fisierul original
    char link_path[MAX_PATH_LENGTH];    // calea completa catre linkul simbolic
} link_info_t;

void* create_symlink_thread(void* arg) {
    link_info_t* info = (link_info_t*)arg;

    if (symlink(info->source_path, info->link_path) == -1) {
        perror("symlink");
    } else {
        printf("Link simbolic creat: %s -> %s\n", info->link_path, info->source_path);
    }

    free(info);
    pthread_exit(NULL);
}

int main(int argc, char* argv[]) {
    if (argc != 2) {
        fprintf(stderr, "Utilizare: %s <director>\n", argv[0]);
        return 1;
    }

    DIR* dir = opendir(argv[1]);
    if (dir == NULL) {
        perror("opendir");
        return 1;
    }

    struct dirent* entry;
    pthread_t threads[1024];
    int thread_count = 0;

    while ((entry = readdir(dir)) != NULL) {
        // Se ignora "." si ".."
        if (strcmp(entry->d_name, ".") == 0 || strcmp(entry->d_name, "..") == 0) {
            continue;
        }

        char filepath[MAX_PATH_LENGTH];
        snprintf(filepath, sizeof(filepath), "%s/%s", argv[1], entry->d_name);

        struct stat st;
        if (stat(filepath, &st) == -1) {
            perror("stat");
            continue;
        }

        if (S_ISREG(st.st_mode)) {
            // Alocam memorie pentru info ce va fi trimisa thread-ului
            link_info_t* info = malloc(sizeof(link_info_t));
            if (!info) {
                perror("malloc");
                continue;
            }

            strncpy(info->source_path, filepath, MAX_PATH_LENGTH);
            snprintf(info->link_path, MAX_PATH_LENGTH, "%s%s", TMP_DIR, entry->d_name);

            // Cream un thread pentru a crea linkul simbolic
            if (pthread_create(&threads[thread_count], NULL, create_symlink_thread, info) != 0) {
                perror("pthread_create");
                free(info);
                continue;
            }

            thread_count++;
        }
    }

    closedir(dir);

    for (int i = 0; i < thread_count; i++) {
        pthread_join(threads[i], NULL);
    }

    printf("Toate link-urile simbolice au fost create.\n");
    return 0;
}


/*
1. Functia de creare link simbolic (create_symlink_thread):

    - Primeste un pointer catre o structura cu doua campuri: calea fisierului original si calea unde va fi creat linkul simbolic
    - Apeleaza symlink() pentru a crea linkul simbolic in /tmp/
    - Afiseaza un mesaj corespunzator in caz de succes sau eroare
    - Elibereaza memoria alocata si inchide thread-ul

2. Functia main:

    - Verifica existenta argumentului pentru director. In lipsa acestuia, afiseaza mesaj de utilizare si iese
    - Deschide directorul si parcurge continutul
    - Pentru fiecare fisier regular gasit:
        - Verifica daca este fisier regulat cu stat()
        - Construieste calea absoluta catre fisier si catre viitorul link
        - Aloca o structura cu aceste informatii si creeaza un thread care va trata linkul
    - Dupa parcurgere, asteapta terminarea tuturor thread-urilor
    - Afiseaza un mesaj final de succes
*/
