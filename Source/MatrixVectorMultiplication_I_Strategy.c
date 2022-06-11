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
    // read vector
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
void printMatrix(int* a, int rows, int cols)
{
    int i, j;

    printf ("\n\nA = \n");
    for (i = 0; i < rows; i++) {
            for (j = 0; j < cols; j++) {
                    printf(" %d", a[i*rows + j]);
            }
            printf ("\n");
    }
    printf ("\n\n");
}


void printVector(char *prompt, int* y, int size)
{

    printf ("\n\n%s\n", prompt);
    for (int i = 0; i < size; i++)
            printf(" %d ", y[i]);
    printf ("\n");
}


int main(int argc, char* argv[]) {
 /* Variable definition */

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
 int tmp, tag, start;

 // input check
 if (argc < 3) {
 	printf("Usage: %s <rows> <cols>\n", argv[0]);
	exit(1);
 } 

 /* MPI initialization */
 MPI_Init(&argc, &argv);
 MPI_Comm_rank(MPI_COMM_WORLD, &menum);
 MPI_Comm_size(MPI_COMM_WORLD, &nproc);
 
 ndim[0] = nproc;
 ndim[1] = 1;
 dim = 2;
 period[0] = period[1] = 1;
 reorder = 0;

 //MPI_Comm grid;
 MPI_Cart_create(MPI_COMM_WORLD, 2, ndim, period, reorder, &grid);
 MPI_Comm_rank(grid, &menum);
 
 
 // start
 if (menum == MASTER) {
 	FILE* text = fopen("inputFile.txt", "r+");
	
	// generate matrix
        generateMatrix(atoi(argv[1]), atoi(argv[2]));
	
	// read matrix
	A = readMatrix(text, &rows, &cols);
        
	// read vector
	x = readVector(text, cols);
        printf("Matrix vector read\n");
 	
	fclose(text);
        
 }
 // broadcast matrix dims
 MPI_Bcast(&rows, 1, MPI_INT, MASTER, grid);
 MPI_Bcast(&cols, 1, MPI_INT, MASTER, grid);
 
 mod = rows%nproc;
 nloc = rows/nproc;
 if (menum < mod) nloc++; 
 
 aloc = (int*)malloc(nloc * cols * sizeof(int));

 // scatter matrix rows
 if (mod) { 
	// matrix rows not divisible by number of processor
 	if (menum == MASTER) {
 		aloc = A;
        	tmp = nloc * cols;
 		start = 0;
 		for (int i = 1; i < nproc; i++) {
 			tag = 22+i;
			start += tmp;
        		if (i==mod) tmp -= cols;
			
			// send matrix rows from MASTER to Pi
			MPI_Send(&A[start], tmp, MPI_INT, i, tag, grid);
		
  		}
 	} else {
 		tag = 22+ menum;
		
		// receive matrix rows from MASTER
		MPI_Recv(aloc, nloc * cols, MPI_INT, MASTER, tag, grid, NULL);
 	}
 } else {
	
	// matrix rows divisible by number of processor
	// send matrix rows from MASTER to others
 	MPI_Scatter(A, nloc * cols, MPI_INT, aloc, nloc * cols, MPI_INT, MASTER, grid);
 }
 
 MPI_Barrier(grid);
 if (menum != MASTER) x = (int*) malloc(cols * sizeof(int)); 
 
 // broadcast vector x
 MPI_Bcast(x, cols, MPI_INT, MASTER, grid);

 // parallel phase
 MPI_Barrier(grid);
 int *yloc = MatrixVectorMultiplication(nloc, cols, aloc, x); 
 int *y = (int *) malloc(rows * sizeof(int));
 int elements;
 if (mod) {
 	if (menum == MASTER) {
		for (int i = 0; i < nloc; i++) y[i] = yloc[i];
		start = nloc;
		for (int i = 1; i < nproc; i++) {
			// receive number of elements
			tag = 2000 + i;
			MPI_Recv(&elements, 1, MPI_INT, i, tag, grid, NULL);
			
			// receive output
			tag = 4500 + i;
			MPI_Recv(y + start, elements, MPI_INT, i, tag, grid, NULL);
			start += elements;
		}
	} else {
		// send number of elements
		tag = 2000 + menum;
		MPI_Send(&nloc, 1, MPI_INT, MASTER, tag, grid);
		
		// send output
		tag = 4500 + menum;
		MPI_Send(yloc, nloc, MPI_INT, MASTER, tag, grid);
	}
 } else {
 	MPI_Gather(yloc, nloc, MPI_INT, y, nloc, MPI_INT, MASTER, grid);
 }
 
 /*
 MPI_Barrier(grid);
 for (int id = 0; id < nproc; id++) {
	if (menum == id) {
		for (int i = 0; i < nloc; i++) for (int j = 0; j < cols; j++) printf("menum %d i=%d j=%d: %d\n", menum, i, j, aloc[i*nloc + j]); 
		for (int i = 0; i < cols; i++) printf("menum %d i=%d: %d\n", menum, i, x[i]);
	 	for (int i = 0; i < nloc; i++) printf("menum %d i=%d: %d\n",menum, i, yloc[i]);
	
	}
	MPI_Barrier(grid);
 }
*/

 if (menum == MASTER) {

	// print matrix
	printMatrix(A, rows, cols);
 	// print vector x
	printVector("x = ", x, cols);
	// print results
 	printVector("y = ", y, rows);
 }

 MPI_Finalize(); 
 return 0;
}



