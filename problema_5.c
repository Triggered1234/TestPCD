/**
Varianta 9 - Problema 5
4.	Scrieti un script C care creeaza un proces fiu. Procesul fiu va apela, asupra programului ls (cu optiunea -l), un apel din familia exec*, asupra unui director specificat pe linia de comanda. Procesul parinte va verifica daca directorul exista inainte de crearea procesului fiu, va astepta terminarea acestuia si va incheia executia.
*/

#include <stdio.h>          // pentru operatii de input/output, cum ar fi printf() si perror()
#include <stdlib.h>         // pentru functii de gestionare a memoriei si operatii generale, cum ar fi exit()
#include <unistd.h>         // pentru apeluri sistem (de exemplu, fork(), execlp())
#include <sys/types.h>      // pentru definirea tipurilor de date pentru manipularea proceselor, cum ar fi pid_t
#include <sys/wait.h>       // pentru operatii de gestionare a proceselor (de exemplu, waitpid(), WIFEXITED())
#include <dirent.h>         // pentru manipularea directoarelor (opendir(), readdir(), closedir())

int main(int argc, char *argv[]) {
    // Verifica daca a fost specificat un argument (directorul)
    if (argc != 2) {  // Daca nu exista exact un argument (care ar trebui sa fie calea directorului)
        fprintf(stderr, "Utilizare: %s <director>\n", argv[0]); 
        return 1; 
    }

    // Deschide directorul pentru a verifica daca exista
    DIR *dir = opendir(argv[1]); 
    if (dir == NULL) {  // Daca directorul nu poate fi deschis (nu exista sau nu avem permisiuni)
        perror("Nu se poate deschide directorul");
        return 1;  // Iesire cu eroare
    }

    // Daca directorul exista, se creeaza procesul fiu
    pid_t pid = fork();  // Creaza un proces fiu
    if (pid == -1) {  // Daca fork-ul esueaza
        perror("Eroare la fork"); 
        closedir(dir);  
        return 1; 
    }

    if (pid == 0) {  // Procesul fiu
        // Executa comanda "ls -l" asupra directorului specificat
        execlp("ls", "ls", "-l", argv[1], (char *)NULL);  // Inlocuieste procesul curent cu executia comenzii "ls -l"
        
        // Daca execlp esueaza, afiseaza eroare
        perror("Eroare la execlp"); 
        exit(1);
    } else {  // Procesul parinte
        // Asteapta terminarea procesului fiu
        int status;
        waitpid(pid, &status, 0); 
        
        // Verifica daca procesul fiu a terminat corect
        if (WIFEXITED(status)) { 
            printf("Procesul fiu s-a terminat cu exit status: %d\n", WEXITSTATUS(status));  
        } else {
            printf("Procesul fiu nu s-a terminat corect\n");
        }

        // Inchide directorul deschis
        closedir(dir);
    }

    return 0;
}
