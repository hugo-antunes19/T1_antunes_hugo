/* mpi_vecadd_gather.c
 * Exercício 5 — Soma de vetores com MPI_Scatter / MPI_Gather.
 *
 * Rank 0 inicializa x[N] = {0, 1, ..., N-1} e y[N] = {0, 2, ..., 2(N-1)}.
 * Distribui blocos via MPI_Scatter, cada rank calcula z_local = x_local + y_local,
 * e rank 0 coleta via MPI_Gather. Verificação: z[i] == 3*i.
 *
 * Compile: mpicc -O2 -Wall -o mpi_vecadd_gather mpi_vecadd_gather.c
 * Run:     mpiexec -n 4 ./mpi_vecadd_gather [N]
 */

#include <stdio.h>
#include <stdlib.h>
#include <mpi.h>

#define DEFAULT_N 16

int main(int argc, char *argv[]) {
    int my_rank, comm_sz, N;
    double *x = NULL, *y = NULL, *z = NULL;

    MPI_Init(&argc, &argv);
    MPI_Comm_rank(MPI_COMM_WORLD, &my_rank);
    MPI_Comm_size(MPI_COMM_WORLD, &comm_sz);

    if (my_rank == 0) {
        N = (argc > 1) ? atoi(argv[1]) : DEFAULT_N;
        if (N % comm_sz != 0) {
            fprintf(stderr, "N=%d must be divisible by p=%d\n", N, comm_sz);
            MPI_Abort(MPI_COMM_WORLD, 1);
        }
    }
    MPI_Bcast(&N, 1, MPI_INT, 0, MPI_COMM_WORLD);

    int local_n = N / comm_sz;
    double *local_x = (double *) malloc(local_n * sizeof(double));
    double *local_y = (double *) malloc(local_n * sizeof(double));
    double *local_z = (double *) malloc(local_n * sizeof(double));

    if (my_rank == 0) {
        x = (double *) malloc(N * sizeof(double));
        y = (double *) malloc(N * sizeof(double));
        z = (double *) malloc(N * sizeof(double));
        for (int i = 0; i < N; i++) {
            x[i] = (double) i;
            y[i] = 2.0 * i;
        }
    }

    /* Distribuir blocos de x e y */
    MPI_Scatter(x, local_n, MPI_DOUBLE,
                local_x, local_n, MPI_DOUBLE, 0, MPI_COMM_WORLD);
    MPI_Scatter(y, local_n, MPI_DOUBLE,
                local_y, local_n, MPI_DOUBLE, 0, MPI_COMM_WORLD);

    /* Computação local: z = x + y */
    for (int i = 0; i < local_n; i++)
        local_z[i] = local_x[i] + local_y[i];

    /* Coletar resultado no rank 0 */
    MPI_Gather(local_z, local_n, MPI_DOUBLE,
               z, local_n, MPI_DOUBLE, 0, MPI_COMM_WORLD);

    if (my_rank == 0) {
        printf("z = x + y (Gather):\n");
        int correct = 1;
        for (int i = 0; i < N; i++) {
            if (N <= 32)
                printf("  z[%2d] = %g  (expected %g)\n", i, z[i], 3.0 * i);
            if (z[i] != 3.0 * i) correct = 0;
        }
        printf("Verificação: %s\n", correct ? "OK" : "ERRO");

        free(x); free(y); free(z);
    }

    free(local_x); free(local_y); free(local_z);
    MPI_Finalize();
    return 0;
}
