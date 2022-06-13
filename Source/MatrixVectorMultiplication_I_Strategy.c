#include "header.c"

int main(int argc, char* argv[]) {
 /* Variable definition */
 srand((unsigned int)0);
 /* MPI_Cart_create variables */
 int menum, nproc;
 int ndim[2];
 int period[2];
 int dim;
 double max; //tempo max
 int reorder;
 double t,t1,t2; //var per raccogliere i tempi dell'algoritmo
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

/*
 if (atoi(argv[1])<nproc)
 {
     printf("Number of rows must be greater or equal than number of processors\n");
     MPI_Finalize();
     exit(1);
 }
*/
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

 //MPI_Barrier(grid);


if (menum != MASTER) x = (int*) malloc(cols * sizeof(int));

 // broadcast vector x
 MPI_Bcast(x, cols, MPI_INT, MASTER, grid);

 // parallel phase
 //MPI_Barrier(grid);
 long long int *yloc = MatrixVectorMultiplication(nloc, cols, aloc, x);
 long long int *y = (long long int *) malloc(rows * sizeof(long long int));

 MPI_Gather(yloc, nloc, MPI_LONG_LONG_INT, y, nloc, MPI_LONG_LONG_INT, MASTER, grid);


 if (menum == MASTER) {

    // print matrix && print vector
    printInput(A,x,(!mod) ? rows : rows-(nproc-mod), cols);

    // print results
    printOutput( y, (!mod) ? rows : rows-(nproc-mod));
 }
 t = t2-t1; //tempo di ogni processore
 MPI_Allreduce(&t, &max,1, MPI_DOUBLE, MPI_MAX, grid); //tutti i processori hanno il massimo tra tutti i tempi
 printf("Il tempo massimo d'esecuzione è %lf\n",max);
 MPI_Finalize();
 return 0;
}



