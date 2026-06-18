/* minmax.c
 * Exercício 4(b) — Máx/mín global com MPI_Reduce.
 *
 * Rank 0 gera vetor de N doubles aleatórios, distribui via MPI_Scatter.
 * Cada rank calcula máx e mín locais. MPI_Reduce com MPI_MAX e MPI_MIN
 * produz o resultado global. Rank 0 verifica contra cálculo serial.
 *
 * Compile: mpicc -O2 -Wall -o minmax minmax.c -lm
 * Run:     mpiexec -n 4 ./minmax [N]
 */

#include <stdio.h>
#include <stdlib.h>
#include <float.h>
#include <math.h>
#include <mpi.h>

#define DEFAULT_N 1000000

int main(int argc, char *argv[]) {
    int    my_rank, comm_sz, N;
    double *data = NULL;

    MPI_Init(&argc, &argv);
    MPI_Comm_rank(MPI_COMM_WORLD, &my_rank);
    MPI_Comm_size(MPI_COMM_WORLD, &comm_sz);

    if (my_rank == 0) {
        N = (argc > 1) ? atoi(argv[1]) : DEFAULT_N;
        if (N % comm_sz != 0) {
            fprintf(stderr, "N=%d deve ser divisível por p=%d\n", N, comm_sz);
            MPI_Abort(MPI_COMM_WORLD, 1);
        }
    }
    MPI_Bcast(&N, 1, MPI_INT, 0, MPI_COMM_WORLD);

    int local_n = N / comm_sz;
    double *local_data = (double *) malloc(local_n * sizeof(double));

    if (my_rank == 0) {
        data = (double *) malloc(N * sizeof(double));
        srand(42);
        for (int i = 0; i < N; i++) {
            data[i] = (double)rand() / RAND_MAX * 1000.0 - 500.0;
        }
    }

    /* Distribuir blocos com MPI_Scatter */
    MPI_Scatter(data,       local_n, MPI_DOUBLE,
                local_data, local_n, MPI_DOUBLE,
                0, MPI_COMM_WORLD);

    /* Calcular máx e mín locais */
    double local_max = local_data[0];
    double local_min = local_data[0];
    for (int i = 1; i < local_n; i++) {
        if (local_data[i] > local_max) local_max = local_data[i];
        if (local_data[i] < local_min) local_min = local_data[i];
    }

    /* Reduzir para máx e mín globais */
    double global_max, global_min;
    MPI_Reduce(&local_max, &global_max, 1, MPI_DOUBLE,
               MPI_MAX, 0, MPI_COMM_WORLD);
    MPI_Reduce(&local_min, &global_min, 1, MPI_DOUBLE,
               MPI_MIN, 0, MPI_COMM_WORLD);

    if (my_rank == 0) {
        /* Verificação serial */
        double serial_max = data[0], serial_min = data[0];
        for (int i = 1; i < N; i++) {
            if (data[i] > serial_max) serial_max = data[i];
            if (data[i] < serial_min) serial_min = data[i];
        }

        printf("N = %d, p = %d\n", N, comm_sz);
        printf("MPI_Reduce  MAX = %g,  MIN = %g\n", global_max, global_min);
        printf("Serial      MAX = %g,  MIN = %g\n", serial_max, serial_min);
        printf("Verificação: MAX %s, MIN %s\n",
               (fabs(global_max - serial_max) < 1e-12) ? "OK" : "ERRO",
               (fabs(global_min - serial_min) < 1e-12) ? "OK" : "ERRO");

        free(data);
    }

    free(local_data);
    MPI_Finalize();
    return 0;
}
