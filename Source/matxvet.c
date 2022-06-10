
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
#include <time.h>

//#include "mpi.h"
#define MAXBUF 1024

// read rows
// read cols
// read elements from text file
void generateMatrix(int rows, int cols) {

    FILE *text = fopen(argv[1], "w+");

    fseek(text, 0, SEEK_SET);
    fprintf(text, "%d\n", rows);
    fprintf(text, "%d\n", cols);

    for (int i = 0; i < rows; i++) {
        for (int j = 0; j < cols; j++) {
            fprintf(text, "%d\n", (int) rand()%587);
        }
    }
    for (int i = 0; i < cols; i++) {
        fprintf(text, "%d\n", (int) rand()%345);
    }

}

int **readMatrix(FILE *text, int *rows, int *cols)
{
}


void createGrid(MPI_Comm *grid, int processorID, int numberOfProcessors, int rows, int cols) {
    int dim = 2, *ndim, reorder = 0, *period;

    ndim = (int*) calloc(dim, sizeof(int));
    // qxp 
    

}

void matXvet_local(MPI_Comm* grid, int processorID, int numberOfProcessor, int nloc, int cols, int* b) {

}

// ./matxvet <textfilename>
int main(int argc, char *argv[])
{
    int rows, cols;
    int byteRead;
    char buffer[MAXBUF];
    int processorID, numberOfProcessor;
    MPI_Status status;
    MPI_Comm grid;
    int nloc, q, p, mod;
    int *xloc;
    // Ax = y
    
    generateMatrix(atoi(argv[2]), atoi(argv[3]));
    
    // read from textfile
    FILE *text = fopen(argv[1], "r");
    fseek(text, 0, SEEK_SET);
    fscanf(text, "%d", &rows);
    fscanf(text, "%d", &cols);

    MPI_Init(&argc, &argv);
    MPI_Comm_rank(MPI_COMM_WORLD, &processorID);
    MPI_Comm_size(MPI_COMM_WORLD, &numberOfProcessor);

    int *A = (int*)malloc(rows * cols * sizeof(int));
    int *x = (int*) malloc(cols * sizeof(int));
    for (int i = 0; i < rows; i++) {
        for (int j = 0; j < cols; j++) {
            fscanf(text, "%d", &A[rows * i + j]);
        }
    }

    // lettura del vettore b
    for (int i = 0; i < cols; i++) {
        fscanf(text, "%d", &x[i]);
    }

   


    /* DISTRIBUZIONE DEI DATI */

    // invio dimensioni matrice
    MPI_Bcast();

    // mod(M,p) != 0 -> OK
    // mod(N,q) != 0 -> ridistribuire il resto delle righe

    if ((mod = rows/q) == 0) nloc = rows/q;
    else {
        for (int step = 0; step < mod; step++)
            nloc++;
    }
    xloc = (int*) malloc(nloc*sizeof(int));


    MPI_Scatter(A, nloc, MPI_INT, xloc, nloc, MPI_INT, 0, /* grid */ MPI_COMM_WORLD);

    // invio dimensioni vettore
    MPI_Bcast();



    /* CALCOLO LOCALE */
    matXvet_local(/* &grid */, processorID, numberOfProcessor, nloc, cols/p, xloc, x)


    return 0;
}