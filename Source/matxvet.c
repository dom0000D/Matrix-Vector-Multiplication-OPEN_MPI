
/*
    MIMD-DM
 Leggiamo da un file le righe, le colonne, la matrice, il vettore b
 mod(N,q) != 0

 Calcolo di nloc
 Distribuzione delle righe a ciascun processore con MPI_Scatter

    1 fase:
 Calcolo in parallelo

    2 fase:
nessuna collezione dei risultati

Implementazione dell'algoritmo parallelo (np processori)
per il calcolo del prodotto tra una Matrice A di dimensione NxM e
un vettore b di dimensione M, adottando la I Strategia.
L'algoritmo è sviluppato in ambiente MPI_DOCKER.

*/

#include <stdio.h>
#include <unistd.h>
#include <stdlib.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
//#include "mpi.h"
#define MAXBUF 1024

// ./matxvet <textfilename>
int main(int argc, char *argv[])
{
    int rows, cols;
    int byteRead;
    char buffer[MAXBUF];
    int processorID, numberOfProcessor;

    FILE *text_fd = fopen("input_file.txt", "r");
    fseek(text_fd, 0, SEEK_SET);

    fscanf(text_fd, "%d", &rows);

    //rows = atoi(buffer);

    printf("buffer: %s\n", buffer);
    printf("number: %d\n", rows);

    return 0;
}