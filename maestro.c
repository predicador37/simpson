#include <stdio.h>
#include <mpi.h>
#include<string.h>

#define A 0
#define B 2
#define ESCLAVOS 3
#define N 100

int main(int argc, char **argv) {
    int rank, size, version, subversion, namelen, universe_size;
    char processor_name[MPI_MAX_PROCESSOR_NAME], worker_program[100];
    MPI_Comm esclavos_comm;
    MPI_Init(&argc, &argv);    /* starts MPI */
    MPI_Comm_rank(MPI_COMM_WORLD, &rank);    /* get current process id */
    MPI_Comm_size(MPI_COMM_WORLD, &size);    /* get number of processes */
    MPI_Get_processor_name(processor_name, &namelen);
    MPI_Get_version(&version, &subversion);
    printf("[maestro] Iniciado proceso maestro %d de %d en %s ejecutando MPI %d.%d\n", rank, size, processor_name,
           version,
           subversion);
    strcpy(worker_program, "./Debug/esclavo");
    MPI_Comm_spawn(worker_program, MPI_ARGV_NULL,ESCLAVOS, MPI_INFO_NULL, 0, MPI_COMM_SELF, &esclavos_comm,
                   MPI_ERRCODES_IGNORE);

    /* Cálculo del número de puntos de intervalo por esclavo */
    int n_esclavo = (N +1)  / ESCLAVOS;
    printf("PUNTOS POR ESCLAVO: %d\n", n_esclavo);

    /* Cálculo de vector de valores de la función */
    double dx = (double)(B - A) / (double)N ;
    double h = ((double)B - (double)A) / (2*(double)N);

    printf("DIFERENCIAL DE X: %f\n", dx);
    int i = 0;
    double y[N+1], y_esclavo[n_esclavo]; // número de puntos = número de intervalos + 1
    double x = (double) A;

    for(i=0;i<N+1;i++){
        y[i]= x * x;
        x+=dx;
        printf("VALOR DE F(X) EN PUNTO i %d: %f\n", i, y[i]);
    }



    MPI_Bcast(&n_esclavo, 1, MPI_INT, MPI_ROOT, esclavos_comm);

    MPI_Scatter(y, n_esclavo, MPI_DOUBLE, y_esclavo, n_esclavo, MPI_DOUBLE, MPI_ROOT, esclavos_comm);

    double suma;
    MPI_Reduce(NULL, &suma, 1, MPI_DOUBLE, MPI_SUM, MPI_ROOT, esclavos_comm);
    printf("SUMA REDUCIDA ES: %f\n", suma);

    double integral = (double) dx/3 * ((double)A*(double)A + suma + (double) B * (double) B);
    printf("RESULTADO DE LA INTEGRAL: %f\n", integral);
    MPI_Comm_disconnect(&esclavos_comm);
    MPI_Finalize();
    return 0;
}