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
#include "mpi.h"
#define MAXBUF 1024

// read rows
// read cols
// read elements from text file
void generateMatrix(int rows, int cols) {

    FILE *text = fopen("inputFile.txt", "w+");

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
    fclose(text);
}

int *readMatrix(FILE *text, int *rows, int *cols)
{
    fseek(text, 0, SEEK_SET);
    fscanf(text, "%d\n", &rows);
    fscanf(text, "%d\n", &cols);
    int *A = (int*)malloc(rows * cols * sizeof(int));

    for (int i = 0; i < rows; i++) {
        for (int j = 0; j < cols; j++) {
                fscanf(text, "%d", &A[rows * i + j]);
        }
    }
    return A;
}
int *readVector(FILE *text, int *cols){
     // lettura del vettore b
    int *x = (int*) malloc(cols * sizeof(int));
    for (int i = 0; i < cols; i++) {
    fscanf(text, "%d", &x[i]);
    }
    return x;
}


void createGrid(MPI_Comm *grid, int q, int p) {
    int dim = 2, *ndim, reorder = 0, *period;
    ndim = (int*) calloc(dim, sizeof(int));
    // qxp 
    ndim[0] = q;
    ndim[1] = p;
    period = (int*) calloc (dim, sizeof(int));
    period[0] = period [1] = 1; 
    MPI_Cart_create(MPI_COMM_WORLD,dim,ndim,period,reorder,grid);
    return;
}

void matXvet_local(MPI_Comm* grid, int processorID, int numberOfProcessor, int nloc, int cols, int* b) {
    

}
void printMat(int *A, int *x){
      printf("\nMatrice\n");
    for(int i = 0; i<rows; i++){
       for(int j = 0; j<cols; j++){
            printf("%d\t",*(A+rows*i+j));
        }
        printf("\n");
       }
    printf("\nVettore\n");
    for(int j = 0; j<cols; j++){
       printf("%d\n", *(x+j));
   }
}

// ./matxvet <textfilename>
int main(int argc, char *argv[])
{
    srand((unsigned int) 0);    
    int rows, cols;
    int byteRead;
    char buffer[MAXBUF];
    int processorID, numberOfProcessor;
    MPI_Status status;
    MPI_Comm grid;
    int nloc, q, p, mod;
    int *xloc;
    // Ax = y
    if(argc < 5){
        printf("Usage %s Rows Cols q p\n",argv[0]);
        exit(-1);
    }
    q = atoi(argv[3]);
    p = atoi(argv[4]);

    generateMatrix(atoi(argv[1]), atoi(argv[2]));

    // read from textfile
    FILE *text= fopen("inputFile.txt", "r");
    

    MPI_Init(&argc, &argv);
    MPI_Comm_rank(MPI_COMM_WORLD, &processorID);
    MPI_Comm_size(MPI_COMM_WORLD, &numberOfProcessor);
    
    if(processorID == 0){
        int *A =readMatrix(text,&rows,&cols);
        int *x = readVector(text,&cols);
        fclose(text);
        printMat(A,x);
        createGrid(&grid,q,p);

        // DISTRIBUZIONE DEI DATI 

        // invio dimensioni matrice
        MPI_Bcast(&rows,1,MPI_INT,0,grid);
        MPI_Bcast(&cols,1,MPI_INT,0,grid);

        // mod(M,p) != 0 -> OK
        // mod(N,q) != 0 -> ridistribuire il resto delle righe

        //Calcolo dimensioni locali
        if ((mod = rows%q) == 0) 
            nloc = rows/q;
        else {
            for (int step = 0; step < mod; step++)
                  nloc++;
            }
        xloc = (int*) malloc(nloc*sizeof(int));


        MPI_Scatter(A, nloc, MPI_INT, xloc, nloc, MPI_INT, 0, grid);
        MPI_Bcast(x,cols,MPI_INT,0,grid);
        } //fine processore master

    // CALCOLO LOCALE 
    matXvet_local(grid, processorID, numberOfProcessor, nloc, cols/p, xloc, x)

   // MPI_Finalize();
    return 0;
}
