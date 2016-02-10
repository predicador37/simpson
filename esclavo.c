//
// Created by predicador on 7/02/16.
//
#include<mpi.h>
#include<stdio.h>
#include <stdlib.h>

#define A 0
#define B 2
#define ESCLAVOS 2
#define N 11

int main(int argc, char **argv) {
    int rank, size, namelen, version, subversion, psize, n_esclavo;
    char processor_name[MPI_MAX_PROCESSOR_NAME];

    MPI_Comm parent;
    MPI_Status stat;
    MPI_Init(&argc, &argv);
    MPI_Comm_rank(MPI_COMM_WORLD, &rank);
    MPI_Comm_size(MPI_COMM_WORLD, &size);
    MPI_Get_processor_name(processor_name, &namelen);
    MPI_Get_version(&version, &subversion);

    printf("[proceso %d] Proceso con identificador %d  de un total de  %d  en  %s  ejecutando  MPI  %d.%d\n", rank,
           rank, size,
           processor_name, version, subversion);

    MPI_Comm_get_parent(&parent);


    if (parent == MPI_COMM_NULL) {
        printf("[proceso %d] Error: no se ha encontrado proceso padre.\n", rank);
        exit(1);
    }

    MPI_Comm_remote_size(parent, &psize);

    if (psize != 1) {

        printf("[proceso %d] Error:  el número de padres debería ser 1 y no %d.\n", rank, psize);
        exit(2);
    }

    MPI_Bcast(&n_esclavo, 1, MPI_INT,0, parent);
    printf("PROCESO %d recibe número de puntos por esclavo: %d\n", rank, n_esclavo);
    double y_esclavo[n_esclavo], y[N+1];
    int j=0;

    MPI_Scatter(NULL, n_esclavo, MPI_DOUBLE, y_esclavo, n_esclavo, MPI_DOUBLE,0, parent);

    int i=0;
    double suma_A=0;
    double suma_B=0;
    printf("Recibiendo...\n");
    for (i=0;i<n_esclavo/2;i++){
        suma_A += y_esclavo[2*i];
        suma_B += y_esclavo[2*i+1];
        printf("PROCESO %d, F(i) es %f\n", rank, y_esclavo[i]);
        printf("PROCESO %d suma A %f\n", rank, suma_A);
        printf("PROCESO %d suma B %f\n", rank, suma_B);
    }
    double suma = 4 * suma_A + 2 * suma_B;

    printf("PROCESO %d suma A %f\n", rank, suma_A);
    printf("PROCESO %d suma B %f\n", rank, suma_B);
    printf("PROCESO %d suma %f\n", rank, suma);

    MPI_Reduce(&suma, NULL, 1, MPI_DOUBLE, MPI_SUM, 0, parent);

    MPI_Comm_disconnect(&parent);
    MPI_Finalize();
    return 0;
}
