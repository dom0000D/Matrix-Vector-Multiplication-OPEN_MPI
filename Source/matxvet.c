
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
//#include "mpi.h"
#define MAXBUF 1024

// ./matxvet <textfilename>
int main(int argc, char *argv[])
{
    int rows, cols;
    int byteRead;
    char buffer;
    int processorID, numberOfProcessor;

    int text_fd = open("input_file.txt", O_RDONLY);
    lseek(text_fd, 0, SEEK_SET);

    if ((byteRead = read(text_fd, &buffer, sizeof(int))) < 0)
    {
        printf("READ ERROR\n");
        exit(-1);
    }

    rows = atoi(buffer);

    printf("buffer: %c\n", buffer);
    printf("number: %d\n", rows) :

                                   return 0;
}