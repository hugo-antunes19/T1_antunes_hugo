/* mpi_psum.c
 * Exercício 3 — Soma Paralela com comunicação ponto-a-ponto.
 *
 * Rank 0 gera vetor de N doubles aleatórios, distribui N/p elementos
 * a cada rank via MPI_Send, cada rank calcula soma local, envia ao
 * rank 0 que reduz e compara com soma serial.
 *
 * Uso: mpiexec -n <p> ./mpi_psum [N]
 *   Default: N = 10000000
 *
 * Compile: mpicc -O2 -Wall -o mpi_psum mpi_psum.c -lm
 */

#include <stdio.h>
#include <stdlib.h>
#include <math.h>
#include <mpi.h>

#define DEFAULT_N 10000000

int main(int argc, char *argv[]) {
    int    my_rank, comm_sz;
    int    N;
    double *data = NULL;

    MPI_Init(&argc, &argv);
    MPI_Comm_rank(MPI_COMM_WORLD, &my_rank);
    MPI_Comm_size(MPI_COMM_WORLD, &comm_sz);

    /* Ler N do argv ou usar default */
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
        /* Gerar vetor aleatório */
        data = (double *) malloc(N * sizeof(double));
        srand(42);
        for (int i = 0; i < N; i++) {
            data[i] = (double)rand() / RAND_MAX;
        }

        /* Distribuir blocos via laço de MPI_Send */
        /* Rank 0 fica com o primeiro bloco */
        for (int i = 0; i < local_n; i++) {
            local_data[i] = data[i];
        }
        for (int dest = 1; dest < comm_sz; dest++) {
            MPI_Send(data + dest * local_n, local_n, MPI_DOUBLE,
                     dest, 0, MPI_COMM_WORLD);
        }
    } else {
        MPI_Recv(local_data, local_n, MPI_DOUBLE,
                 0, 0, MPI_COMM_WORLD, MPI_STATUS_IGNORE);
    }

    /* Soma local */
    double local_sum = 0.0;
    for (int i = 0; i < local_n; i++) {
        local_sum += local_data[i];
    }

    /* Coletar somas parciais no rank 0 via Send/Recv */
    if (my_rank != 0) {
        MPI_Send(&local_sum, 1, MPI_DOUBLE, 0, 1, MPI_COMM_WORLD);
    } else {
        double total_parallel = local_sum;
        double partial;
        for (int src = 1; src < comm_sz; src++) {
            MPI_Recv(&partial, 1, MPI_DOUBLE, src, 1,
                     MPI_COMM_WORLD, MPI_STATUS_IGNORE);
            total_parallel += partial;
        }

        /* Soma serial para verificação */
        double total_serial = 0.0;
        for (int i = 0; i < N; i++) {
            total_serial += data[i];
        }

        double rel_error = fabs(total_parallel - total_serial) / fabs(total_serial);

        printf("N = %d, p = %d\n", N, comm_sz);
        printf("Soma serial   = %.15e\n", total_serial);
        printf("Soma paralela = %.15e\n", total_parallel);
        printf("Erro relativo = %.4e\n", rel_error);
    }

    free(local_data);
    if (my_rank == 0) free(data);

    MPI_Finalize();
    return 0;
}
