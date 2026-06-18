/* mpi_vecadd_allgather.c
 * Exercício 5 — Soma de vetores com MPI_Scatter / MPI_Allgather.
 *
 * Mesma lógica do mpi_vecadd_gather.c, mas usa MPI_Allgather em vez
 * de MPI_Gather, de modo que TODOS os ranks terminam com o vetor z
 * completo.
 *
 * Quando Allgather é útil: quando todos os processos precisam do
 * resultado completo para computação subsequente (ex: multiplicação
 * matriz-vetor iterativa, onde cada rank precisa do vetor x completo
 * para computar y = A*x na próxima iteração).
 *
 * Compile: mpicc -O2 -Wall -o mpi_vecadd_allgather mpi_vecadd_allgather.c
 * Run:     mpiexec -n 4 ./mpi_vecadd_allgather [N]
 */

#include <stdio.h>
#include <stdlib.h>
#include <mpi.h>

#define DEFAULT_N 16

int main(int argc, char *argv[]) {
    int my_rank, comm_sz, N;
    double *x = NULL, *y = NULL;

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
    /* Todos os ranks alocam z completo */
    double *z = (double *) malloc(N * sizeof(double));

    if (my_rank == 0) {
        x = (double *) malloc(N * sizeof(double));
        y = (double *) malloc(N * sizeof(double));
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

    /* Allgather: todos os ranks recebem z completo */
    MPI_Allgather(local_z, local_n, MPI_DOUBLE,
                  z, local_n, MPI_DOUBLE, MPI_COMM_WORLD);

    /* Cada rank verifica o resultado */
    int correct = 1;
    for (int i = 0; i < N; i++) {
        if (z[i] != 3.0 * i) correct = 0;
    }

    if (N <= 32) {
        /* Imprimir de cada rank (serializado para clareza) */
        for (int r = 0; r < comm_sz; r++) {
            if (my_rank == r) {
                printf("[Rank %d] z = x + y (Allgather):\n", my_rank);
                for (int i = 0; i < N; i++)
                    printf("  z[%2d] = %g\n", i, z[i]);
                printf("  Verificação: %s\n", correct ? "OK" : "ERRO");
                fflush(stdout);
            }
            MPI_Barrier(MPI_COMM_WORLD);
        }
    } else {
        if (my_rank == 0) {
            printf("N = %d, p = %d\n", N, comm_sz);
            printf("Todos os ranks possuem z completo.\n");
        }
        /* Apenas rank 0 reporta verificação consolidada */
        int all_correct;
        MPI_Reduce(&correct, &all_correct, 1, MPI_INT,
                   MPI_MIN, 0, MPI_COMM_WORLD);
        if (my_rank == 0) {
            printf("Verificação global: %s\n",
                   all_correct ? "OK (todos ranks)" : "ERRO");
        }
    }

    free(local_x); free(local_y); free(local_z); free(z);
    if (my_rank == 0) { free(x); free(y); }

    MPI_Finalize();
    return 0;
}
