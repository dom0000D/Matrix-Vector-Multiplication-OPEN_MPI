#ifndef HEADER_H
#define HEADER_H
#include <stdio.h>
#include <stdlib.h>
#include "mpi.h"
#include <unistd.h>
#include <fcntl.h>
#include <limits.h>
#include <time.h>
#include <string.h>
#define MASTER 0

void generateMatrix(int rows, int cols);
int *readMatrix(FILE *text, int *rows, int *cols, int nproc, int *mod);
int *readVector(FILE *text, int cols);
long long int* MatrixVectorMultiplication(int nloc, int cols, int* aloc, int* x);
void printInput(int* a,int *x ,int rows, int cols);
void printOutput( long long int* y, int size);

#endif /*HEADER_H*/