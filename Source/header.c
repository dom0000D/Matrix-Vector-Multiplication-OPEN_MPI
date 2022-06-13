#ifndef HEADER_C
#define HEADER_C
#include "header.h"

// generate matrix
//  input
//  	rows: matrix rows
//  	cols: matrix cols, vector rows
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

// read matrix from text file
// input
// 	text: pointer to a text file stream
// 	nproc: number of processor
// input/output
// 	rows: address of a variable which stores matrix rows
// 	cols: address of a variable which stores matrix cols, vector rows
// output:
// 	A: address to a matrix stacked 1D nxm-sized
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


// read vector from textfile
// input:
// 	text: pointer to a textfile stream
// 	rows: vector size 
// output:
// 	x: address of an array 1D m-sized
int *readVector(FILE *text, int rows){
    // read vector
    int *x = (int*) malloc(rows * sizeof(int));
    for (int i = 0; i < rows; i++) {
    fscanf(text, "%d", &x[i]);
    }
    return x;
}

// compute matrix-vector multiplication
// input
// 	nloc: number of rows assigned to each processor
// 	cols: matrix A number of cols, vector x size 
// 	aloc: address of a local vector which stores elements of matrix rows assigned
// 	x: address of an array 1D which stores elements of vector x
// output
// 	y: address of an array 1D nloc-sized
long long int* MatrixVectorMultiplication(int nloc, int cols, int* aloc, int* x) {
    MPI_Barrier(grid);
    t1 = MPI_Wtime();
    long long int *y = (long long int*) calloc(nloc, sizeof(long long int)) ;
    for (int i = 0; i < nloc; i++)
        for (int j = 0; j < cols; j++)
            y[i] += aloc[i*nloc + j] * x[j];

    MPI_Barrier(grid);
    t2 = MPI_Wtime();
    return y;
}

// print input of matrix-vector multiplication problem
// input
// 	a: address of matrix A
// 	x: address of vector z
// 	rows: matrix A number of rows
// 	cols: matrix A number of rows, vector x size
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

// print output of matrix-vector multiplication problem
// input
// 	y: address to an array 1D which stores the result of matrix-vector multiplication
// 	size: number of elements of vector y
void printOutput( long long int* y, int size)
{

    printf ("\n\ny=\n");
    for (int i = 0; i < size; i++)
            printf(" %lld ", y[i]);
    printf ("\n");
}

#endif /*HEADER_C*/
