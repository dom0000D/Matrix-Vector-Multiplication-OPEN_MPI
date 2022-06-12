#ifndef HEADER_C
#define HEADER_C
#include "header.h"

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

#endif /*HEADER_C*/