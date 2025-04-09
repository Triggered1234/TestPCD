/**
Varianta 9 - Problema 4
3.	Scrieti un script C care preia ca argument pe linia de comanda un director. Scriptul va deschide directorul, va traversa si selecta DOAR fisierele regulare din acesta, iar pentru fiecare din acestea, va crea un hard link in /tmp/<nume_fisier> unde nume_fisier este numele fisierului original. Pentru fiecare fisier regular din directorul dat, se va crea un thread separat, care va fi responsabil de crearea linkului hard. La final, se va astepta terminarea tuturor thread-urilor.
*/

#include <stdio.h>          // pentru operatii de input/output, cum ar fi printf() si perror()
#include <stdlib.h>         // pentru functii de gestionare a memoriei si operatii generale, cum ar fi exit()
#include <dirent.h>         // pentru manipularea directoarelor, cum ar fi opendir(), readdir(), closedir()
#include <sys/types.h>      // pentru definirea tipurilor de date utilizate pentru manipularea fisierelor (de exemplu, struct stat)
#include <sys/stat.h>       // pentru definirea structurii 'stat' care este folosita pentru a obtine informatii despre fisiere
#include <unistd.h>         // pentru apeluri sistem (de exemplu, link(), care este folosita pentru a crea hard link-uri)
#include <pthread.h>        // pentru thread-uri (multithreading), care permite crearea si gestionarea thread-urilor
#include <fcntl.h>          // pentru operatii de deschidere a fisierelor (nu este folosita direct in acest cod, dar este incluzita pentru operatiuni suplimentare cu fisiere)
#include <string.h>         // pentru manipularea string-urilor, cum ar fi functia snprintf() si strrchr()

#define TMP_DIR "/tmp/"      // Directorul in care vor fi create hard link-urile. Aici sunt plasate link-urile
#define MAX_PATH_LENGTH 1024  // Dimensiunea buffer-ului pentru stocarea cailor de fisiere. Este setata la 1024 pentru a preveni trunchierea


// Functie care va fi executata de fiecare thread pentru a crea un hard link
void* create_hard_link(void* arg) {
    char* filepath = (char*) arg;         
    struct stat file_stat;

    // Verifica daca fisierul este accesibil si obtine informatii despre acesta
    if (stat(filepath, &file_stat) == -1) {
        perror("stat");                  // Daca stat() esueaza, afiseaza eroarea si inchide thread-ul
        pthread_exit(NULL);
    }

    // Verifica daca este fisier regulat (nu un director sau alt tip de fisier)
    if (S_ISREG(file_stat.st_mode)) {
        char tmp_link_path[MAX_PATH_LENGTH];   // Buffer pentru calea noului hard link

        // Creaza calea completa pentru hard link in directorul TMP_DIR
        snprintf(tmp_link_path, sizeof(tmp_link_path), "%s%s", TMP_DIR, strrchr(filepath, '/') + 1);

        // Incearca sa creeze hard link-ul folosind functia 'link'
        if (link(filepath, tmp_link_path) == -1) {
            perror("link");              // Daca 'link' esueaza, afiseaza eroarea
        } else {
            // Daca hard link-ul este creat cu succes, afiseaza mesajul
            printf("Hard link creat: %s -> %s\n", tmp_link_path, filepath);
        }
    }

    pthread_exit(NULL);  // Inchide thread-ul dupa ce a terminat
}

int main(int argc, char* argv[]) {
    // Verifica daca a fost oferit un argument pentru director
    if (argc != 2) {
        fprintf(stderr, "Utilizare: %s <director>\n", argv[0]);  // Afiseaza mesajul de utilizare
        return 1;
    }

    // Deschide directorul pentru a-l parcurge
    DIR* dir = opendir(argv[1]);
    if (dir == NULL) {
        perror("opendir");  // Daca deschiderea directorului esueaza, afiseaza eroarea
        return 1;
    }

    struct dirent* entry;               // Variabila pentru a stoca informatiile fiecarui fisier din director
    pthread_t threads[1024];            // Tablica de thread-uri (fiecare fisier va avea un thread)
    int thread_count = 0;               // Contor pentru a numara thread-urile create

    // Parcurge directorul si creeaza un thread pentru fiecare fisier regulat
    while ((entry = readdir(dir)) != NULL) {
        char filepath[MAX_PATH_LENGTH];  // Buffer pentru calea completa a fisierului

        // Verifica daca concatenarea directorului si numelui fisierului depaseste dimensiunea bufferului
        if (strlen(argv[1]) + strlen(entry->d_name) + 1 > MAX_PATH_LENGTH) {
            fprintf(stderr, "Calea fisierului este prea lunga: %s/%s\n", argv[1], entry->d_name);
            continue;  // Daca calea este prea lunga, sare peste acest fisier
        }

        // Creeaza calea completa a fisierului
        snprintf(filepath, sizeof(filepath), "%s/%s", argv[1], entry->d_name);

        // Verifica daca este fisier regulat (nu un director sau alt tip de fisier)
        if (entry->d_type == DT_REG) {
            // Creeaza un thread pentru fiecare fisier regulat
            if (pthread_create(&threads[thread_count], NULL, create_hard_link, (void*)filepath) != 0) {
                perror("pthread_create");  // Daca crearea thread-ului esueaza, afiseaza eroarea
                closedir(dir);  // Inchide directorul inainte de a iesi din program
                return 1;
            }
            thread_count++;  // Creste contorul de thread-uri
        }
    }

    closedir(dir);  // Inchide directorul dupa ce am parcurs toate fisierele

    // Asteapta terminarea tuturor thread-urilor
    for (int i = 0; i < thread_count; i++) {
        pthread_join(threads[i], NULL);  // Asteapta ca fiecare thread sa termine
    }

    printf("Toate link-urile au fost create.\n");
    return 0;
}
