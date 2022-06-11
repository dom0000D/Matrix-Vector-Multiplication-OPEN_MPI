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
            fprintf(text, "%d\n", (int) 1 + rand()%4);
        }
    }
    for (int i = 0; i < cols; i++) {
        fprintf(text, "%d\n", (int) 1+ rand()%4);
    }
    fclose(text);
}

int *readMatrix(FILE *text, int *rows, int *cols)
{
    fseek(text, 0, SEEK_SET);
    fscanf(text, "%d\n", rows);
    fscanf(text, "%d\n", cols);
    int *A = (int*)malloc((*rows) * (*cols) * sizeof(int));
    printf("rows: %d\tcols: %d\n", *rows, *cols);
    for (int i = 0; i < *rows; i++) {
        for (int j = 0; j < *cols; j++) {
                fscanf(text, "%d", &A[*rows * i + j]);
        }
    }
    return A;
}
int *readVector(FILE *text, int cols){
     // lettura del vettore b
    int *x = (int*) malloc(cols * sizeof(int));
    for (int i = 0; i < cols; i++) {
    fscanf(text, "%d", &x[i]);
    }
    return x;
}


int* MatrixVectorMultiplication(int nloc, int cols, int* aloc, int* x) {
	int *y = (int*) calloc(nloc, sizeof(int)) ;
	for (int i = 0; i < nloc; i++)
		for (int j = 0; j < cols; j++)
			y[i] += aloc[i*nloc + j] * x[j];
	
	return y;
}
int main(int argc, char* argv[]) {
 int menum, nproc;
 int ndim[2]; 
 int period[2];
 int dim;
 MPI_Init(&argc, &argv);
 MPI_Comm_rank(MPI_COMM_WORLD, &menum);
 MPI_Comm_size(MPI_COMM_WORLD, &nproc);
 
 ndim[0] = nproc;
 ndim[1] = 1;
 dim = 2;
 period[0] = period[1] = 1;
 int reorder = 0;
 MPI_Comm grid;
 MPI_Cart_create(MPI_COMM_WORLD, 2, ndim, period, reorder, &grid);
 MPI_Comm_rank(grid, &menum);
 
 
 // start
 int rows, cols;
 int *A, *x;
 if (menum == MASTER) {
 	FILE* text = fopen("inputFile.txt", "r+");
        generateMatrix(atoi(argv[1]), atoi(argv[2]));
	
	A = readMatrix(text, &rows, &cols);
        x = readVector(text, cols);
        printf("Matrix vector read\n");
 	fclose(text);
        
 }

 
 MPI_Bcast(&rows, 1, MPI_INT, MASTER, grid);
 MPI_Bcast(&cols, 1, MPI_INT, MASTER, grid);
 
 
 int mod, nloc, *aloc;
 mod = rows%nproc;
 nloc = rows/nproc;
 if (menum < mod) nloc++; 
 printf("nloc: %d\n", nloc); 
 aloc = (int*)malloc(nloc * cols * sizeof(int));
 int tmp, start;
 int tag;
 if (menum == MASTER) {
 	aloc = A;
        tmp = nloc * cols;
 	start = 0;
 	for (int i = 1; i < nproc; i++) {
 		tag = 22+i;
		start += tmp;
        	if (i==mod) tmp -= cols;
		MPI_Send(&A[start], tmp, MPI_INT, i, tag, grid);
		//MPI_Scatter(&A[start], tmp, MPI_INT, aloc, tmp, MPI_INT, MASTER, grid);
  }
 } else {
 	tag = 22+ menum;
	MPI_Recv(aloc, nloc * cols, MPI_INT, MASTER, tag, grid, NULL);
 }

 MPI_Barrier(grid);
 int* xloc = (int*) malloc(cols * sizeof(int));

 if (menum == MASTER) {
	for (int i = 1; i < nproc; i++) {
		tag = 88+i;
		MPI_Send(x, cols, MPI_INT, i, tag, grid);
 	}
} else {
	tag = 88+menum;
 	MPI_Recv(xloc, cols, MPI_INT, MASTER, tag, grid, NULL);
 }
//for (int i = 0; i < cols; i++) MPI_Bcast(&x[i], 1, MPI_INT, 0, grid);
 

 printf("menum %d aloc: ", menum);
 for (int j = 0; j < nloc; j++) {
        for (int k = 0; k < cols; k++) {
                printf("j=%d k=%d : %d\t",j, k, aloc[j*nloc + k]);
        }
        printf("\n");
 }
 printf("menum %d xloc: ", menum);
 for (int j = 0; j < cols; j++) {
	if (menum != MASTER)
        printf("j= %d: %d\t", j, xloc[j]);
	else printf("j= %d: %d\t", j, x[j]); 
}


 // calcolo locale
 int *y = MatrixVectorMultiplication(nloc, cols, aloc, (menum == MASTER) ? x : xloc);
 MPI_Barrier(grid);

 // print
 printf("menum %d output: \n", menum);
 for (int i = 0; i < nproc; i++) {
	if (menum == i) {
		for (int j = 0; j < nloc; j++) {
			printf("j=%d : %d\n", j, y[j]);
		}
	}
 } 
 printf("\n");

 MPI_Finalize(); 
 return 0;
}


