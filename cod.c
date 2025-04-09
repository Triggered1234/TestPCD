
#include <stdio.h>  // Utilizat pentru printf si scanf
#include <stdlib.h> // Utilizat pentru malloc, free, perror
#include <pthread.h> // Utilizat pentru gestionarea thread-urilor
#include <dirent.h>  // Utilizat pentru manipularea directoarelor
#include <sys/stat.h> // Utilizat pentru stat() si mkdir() - pentru a obtine informatii despre fisiere si pentru a crea directoare
#include <string.h>  // Utilizat pentru manipularea string-urilor
#include <unistd.h>  // Utilizat pentru open(), read(), write(), link()
#include <fcntl.h>   // Utilizat pentru gestionarea fisierelor (O_RDONLY, O_WRONLY)

// Structura pentru argumentele de intrare pentru thread1
typedef struct inFile {
    char *filePath;
    char *buffer;
    size_t size;
} inFile_t;

// Structura pentru argumentele de iesire pentru thread2
typedef struct outFile {
    char *filePath;
    char *buffer;
    size_t size;
} outFile_t;

// Functia pentru citirea datelor din fisier folosind apeluri sistem
size_t myRead(char *path, char **buffer, size_t size) {
    int fd = open(path, O_RDONLY); // Deschidem fisierul in mod read-only
    if (fd == -1) {
        perror("Eroare la deschiderea fisierului de citire"); // Tratarea erorii daca fisierul nu se deschide
        return 0;
    }

    size_t bytesRead = read(fd, *buffer, size);
    if (bytesRead == -1) {
        perror("Eroare la citirea din fisier"); // Tratarea erorii la citire
    }

    close(fd);
    return bytesRead;
}

// Functia pentru scrierea datelor in fisier folosind apeluri sistem
size_t myWrite(char *path, char *buffer, size_t size) {
    int fd = open(path, O_WRONLY | O_CREAT | O_TRUNC, 0644); // Deschidem fisierul in mod write-only
    if (fd == -1) {
        perror("Eroare la deschiderea fisierului de scriere"); // Tratarea erorii daca fisierul nu se deschide
        return 0;
    }

    size_t bytesWritten = write(fd, buffer, size); // Scriem datele din buffer in fisier
    if (bytesWritten == -1) {
        perror("Eroare la scrierea in fisier"); // Tratarea erorii la scriere
    }

    close(fd);
    return bytesWritten;
}

// Thread-ul pentru citirea fisierului
void *inThread(void *arg) {
    inFile_t *file = (inFile_t *)arg;

    // Citim datele din fisier folosind myRead()
    size_t bytesRead = myRead(file->filePath, &file->buffer, file->size);

    if (bytesRead == 0) {
        printf("Eroare la citirea fisierului %s\n", file->filePath); // Daca citirea a esuat, afisam un mesaj de eroare
        return NULL;
    }

    printf("Date citite: %s\n", file->buffer);
    return NULL;
}

// Thread-ul pentru scrierea in fisier
void *outThread(void *arg) {
    outFile_t *file = (outFile_t *)arg;

    // Scriem datele in fisier folosind myWrite()
    size_t bytesWritten = myWrite(file->filePath, file->buffer, file->size);

    if (bytesWritten == 0) {
        printf("Eroare la scrierea in fisierul %s\n", file->filePath); // Daca scrierea a esuat, afisam un mesaj de eroare
        return NULL;
    }

    printf("Datele au fost scrise in fisierul %s\n", file->filePath);
    return NULL;
}

// Thread pentru extragerea de informatii despre fisiere
void *fileThread(void *arg) {
    char *filename = (char *)arg;
    struct stat fileStat;

    if (stat(filename, &fileStat) == -1) {
        perror("stat failed"); // Tratarea erorii daca stat() esueaza
        pthread_exit(NULL);
    }

    // Afisam numele si dimensiunea fisierului
    printf("\nFisier: %s \n", filename);
    printf("Dimensiune: %ld bytes\n", fileStat.st_size);

    // Afisam permisiunile fisierului
    printf("Permisiuni: ");
    printf((S_ISDIR(fileStat.st_mode)) ? "d" : "-");
    printf((fileStat.st_mode & S_IRUSR) ? "r" : "-");
    printf((fileStat.st_mode & S_IWUSR) ? "w" : "-");
    printf((fileStat.st_mode & S_IXUSR) ? "x" : "-");
    printf((fileStat.st_mode & S_IRGRP) ? "r" : "-");
    printf((fileStat.st_mode & S_IWGRP) ? "w" : "-");
    printf((fileStat.st_mode & S_IXGRP) ? "x" : "-");
    printf((fileStat.st_mode & S_IROTH) ? "r" : "-");
    printf((fileStat.st_mode & S_IWOTH) ? "w" : "-");
    printf((fileStat.st_mode & S_IXOTH) ? "x" : "-");
    printf("\n");

    // Afisam alte informatii despre fisier
    printf("Inode: %ld\n", fileStat.st_ino);
    printf("Ultima modificare: %s", ctime(&fileStat.st_mtime));
    printf("Ultima accesare: %s", ctime(&fileStat.st_atime));

    free(arg);
    pthread_exit(NULL);
}

// Functie pentru a crea un director doar daca nu exista
int createDirectoryIfNotExists(const char *dirPath) {
    struct stat st = {0};
    if (stat(dirPath, &st) == -1) {
        // Daca nu exista, il cream cu permisiuni 0755
        if (mkdir(dirPath, 0755) == -1) {
            perror("Eroare la crearea directorului");
            return -1;
        }
    }
    return 0;
}

// Functie pentru a verifica daca un fisier este un director si a-l sterge daca este
int createFileIfDirectory(const char *path) {
    struct stat st;
    if (stat(path, &st) == 0) {
        if (S_ISDIR(st.st_mode)) {
            if (rmdir(path) == -1) {
                perror("Eroare la stergerea directorului");
                return -1;
            }
        }
    }
    return 0;
}

int main() {
    pthread_t thr1, thr2, fileThreads[256];  // Declaram thread-urile pentru citire, scriere si fisierul
    inFile_t inputFile;
    outFile_t outputFile;
    DIR *dir;  // Pointer pentru director
    struct dirent *entry;  // Structura pentru intrarea curenta din director
    int threadCount = 0;  // Contor pentru numarul de thread-uri create

    inputFile.filePath = malloc(256);  // Alocam memorie pentru calea fisierului de citire
    outputFile.filePath = malloc(256);  // Alocam memorie pentru calea fisierului de iesire

    printf("Introduceti calea fisierului de citire: ");
    scanf("%s", inputFile.filePath);

    printf("Cati bytes doriti sa cititi din fisier? ");
    scanf("%zu", &inputFile.size);

    inputFile.buffer = malloc(inputFile.size);
    if (inputFile.buffer == NULL) {
        perror("Memorie insuficienta pentru buffer");
        return 1;
    }

    // Crearea thread-ului pentru citirea din fisier
    pthread_create(&thr1, NULL, inThread, (void *)&inputFile);
    pthread_join(thr1, NULL);  // Asteptam sa se termine thread-ul de citire

    // Citirea datelor pentru scriere
    printf("Introduceti calea fisierului de iesire: ");
    scanf("%s", outputFile.filePath);

    outputFile.buffer = inputFile.buffer;
    outputFile.size = inputFile.size;

    // Crearea thread-ului pentru scrierea in fisier
    pthread_create(&thr2, NULL, outThread, (void *)&outputFile);
    pthread_join(thr2, NULL);  // Asteptam sa se termine thread-ul de scriere

    // Crearea directoarelor si fisierelor
    if (createDirectoryIfNotExists("./d1") == -1) {
        return 1;
    }
    if (createDirectoryIfNotExists("./d1/d2") == -1) {
        return 1;
    }
    if (createDirectoryIfNotExists("./d1/d2/d3") == -1) {
        return 1;
    }