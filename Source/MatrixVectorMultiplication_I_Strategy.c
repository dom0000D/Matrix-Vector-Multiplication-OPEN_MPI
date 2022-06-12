#include <stdio.h>
#include <stdlib.h>
#include "mpi.h"
#include <unistd.h>
#include <fcntl.h>
#include <limits.h>
#include <time.h>
#include <string.h>
#define MASTER 0
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
            fprintf(text, "%d\n", (int) 1 + rand()%UCHAR_MAX);
        }
    }
    for (int i = 0; i < cols; i++) {
        fprintf(text, "%d\n", (int) 1+ rand()%UCHAR_MAX);
    }
    fclose(text);
}
int *readMatrix(FILE *text, int *rows, int *cols, int nproc, int *mod)
{

    fseek(text, 0, SEEK_SET);
    fscanf(text, "%d\n", rows);
    fscanf(text, "%d\n", cols);
    *mod = *rows%nproc;
    int new_size = (*mod) ? *rows + (nproc- *mod) : *rows; //padding
    int *A = (int*) calloc(new_size*(*cols) , sizeof(int));
    for (int i = 0; i < *rows; i++) {
        for (int j = 0; j < *cols; j++) {
                fscanf(text, "%d", &A[*cols * i + j]);
        }

    }

    *rows = new_size;
    return A;
}

int *readVector(FILE *text, int cols){
    // read vector
    int *x = (int*) malloc(cols * sizeof(int));
    for (int i = 0; i < cols; i++) {
    fscanf(text, "%d", &x[i]);
    }
    return x;
}
long long int* MatrixVectorMultiplication(int nloc, int cols, int* aloc, int* x) {
    long long int *y = (long long int*) calloc(nloc, sizeof(long long int)) ;
    for (int i = 0; i < nloc; i++)
        for (int j = 0; j < cols; j++)
            y[i] += aloc[i*nloc + j] * x[j];

    return y;
}
void printInput(int* a,int *x ,int rows, int cols)
{
    int i, j;

    printf ("\n\nA = \n");
    for (i = 0; i < rows; i++) {
            for (j = 0; j < cols; j++) {
                    printf(" %d\t", *(a +i*cols+j));
            }
            printf ("\n");
    }
    printf ("\n\n");
    printf ("\nx=\n");
    for (int i = 0; i < cols; i++)
            printf(" %d\t", x[i]);
    printf ("\n");
}
void printOutput( long long int* y, int size)
{

    printf ("\n\ny=\n");
    for (int i = 0; i < size; i++)
            printf(" %lld ", y[i]);
    printf ("\n");
}

int main(int argc, char* argv[]) {
 /* Variable definition */
 srand((unsigned int)0);
 /* MPI_Cart_create variables */
 int menum, nproc;
 int ndim[2];
 int period[2];
 int dim;
 int reorder;
 MPI_Comm grid;

 /* Program variables */
 int rows, cols;
 int *A, *x;
 FILE *text;
 int mod, nloc, *aloc, *xloc;

 // input check
 if (argc < 3) {
    printf("Usage: %s <rows> <cols>\n", argv[0]);
    exit(1);
 }


 /* MPI initialization */
 MPI_Init(&argc, &argv);
 MPI_Comm_rank(MPI_COMM_WORLD, &menum);
 MPI_Comm_size(MPI_COMM_WORLD, &nproc);

 if (atoi(argv[1])<nproc)
 {
     printf("Number of rows must be greater or equal than number of processors\n");
     MPI_Finalize();
     exit(-1);
 }

 ndim[0] = nproc;
 ndim[1] = 1;
 dim = 2;
 period[0] = period[1] = 1;
 reorder = 0;

 MPI_Cart_create(MPI_COMM_WORLD, 2, ndim, period, reorder, &grid);
 MPI_Comm_rank(grid, &menum);
 // start
 if (menum == MASTER) {
    FILE* text = fopen("inputFile.txt", "r+");

    // generate matrix
        generateMatrix(atoi(argv[1]), atoi(argv[2]));

    // read matrix
    A = readMatrix(text, &rows, &cols, nproc,&mod);

    // read vector
    x = readVector(text, cols);

    fclose(text);

 }

 // broadcast matrix dims
 MPI_Bcast(&rows, 1, MPI_INT, MASTER, grid);
 MPI_Bcast(&cols, 1, MPI_INT, MASTER, grid);

 nloc = rows/nproc;
 aloc = (int*)malloc(nloc * cols * sizeof(int));

 // scatter matrix rows

 // matrix rows divisible by number of processor
 // send matrix rows from MASTER to others
 MPI_Scatter(A, nloc * cols, MPI_INT, aloc, nloc * cols, MPI_INT, MASTER, grid);

 MPI_Barrier(grid);


if (menum != MASTER) x = (int*) malloc(cols * sizeof(int));

 // broadcast vector x
 MPI_Bcast(x, cols, MPI_INT, MASTER, grid);

 // parallel phase
 MPI_Barrier(grid);
 long long int *yloc = MatrixVectorMultiplication(nloc, cols, aloc, x);
 long long int *y = (long long int *) malloc(rows * sizeof(long long int));

 MPI_Gather(yloc, nloc, MPI_LONG_LONG_INT, y, nloc, MPI_LONG_LONG_INT, MASTER, grid);


 if (menum == MASTER) {

    // print matrix && print vector
    printInput(A,x,(!mod) ? rows : rows-(nproc-mod), cols);

    // print results
    printOutput( y, (!mod) ? rows : rows-(nproc-mod));
 }

 MPI_Finalize();
 return 0;
}




