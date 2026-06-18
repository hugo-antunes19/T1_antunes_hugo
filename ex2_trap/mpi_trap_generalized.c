/* mpi_trap_generalized.c
 * Exercício 2 — Regra do Trapézio Generalizada.
 *
 * (a) Balanceamento de carga para n arbitrário (p não precisa dividir n).
 * (b) Estudo de escalabilidade forte com MPI_Wtime.
 * (c) Bônus: convergência O(h²).
 *
 * Uso: mpiexec -n <p> ./mpi_trap_generalized <a> <b> <n>
 *   Ex: mpiexec -n 3 ./mpi_trap_generalized 0 3.14159265358979323846 10
 *
 * Compile: mpicc -O2 -Wall -o mpi_trap_generalized mpi_trap_generalized.c -lm
 */

#include <stdio.h>
#include <stdlib.h>
#include <math.h>
#include <mpi.h>

/* Função a integrar: sin(x).  Integral em [0, pi] = 2. */
static double f(double x) { return sin(x); }

double Trap(double a, double b, int n) {
    double h = (b - a) / n;
    double sum = (f(a) + f(b)) / 2.0;
    for (int i = 1; i <= n - 1; i++) {
        double x_i = a + i * h;
        sum += f(x_i);
    }
    return h * sum;
}

int main(int argc, char *argv[]) {
    int    my_rank, comm_sz;
    double a, b, h;
    int    n;

    MPI_Init(&argc, &argv);
    MPI_Comm_rank(MPI_COMM_WORLD, &my_rank);
    MPI_Comm_size(MPI_COMM_WORLD, &comm_sz);

    /* Ler argumentos no rank 0 e distribuir via Send/Recv manual */
    if (my_rank == 0) {
        if (argc != 4) {
            fprintf(stderr, "Usage: %s <a> <b> <n>\n", argv[0]);
            MPI_Abort(MPI_COMM_WORLD, 1);
        }
        a = atof(argv[1]);
        b = atof(argv[2]);
        n = atoi(argv[3]);

        for (int dest = 1; dest < comm_sz; dest++) {
            MPI_Send(&a, 1, MPI_DOUBLE, dest, 0, MPI_COMM_WORLD);
            MPI_Send(&b, 1, MPI_DOUBLE, dest, 0, MPI_COMM_WORLD);
            MPI_Send(&n, 1, MPI_INT,    dest, 0, MPI_COMM_WORLD);
        }
    } else {
        MPI_Recv(&a, 1, MPI_DOUBLE, 0, 0, MPI_COMM_WORLD, MPI_STATUS_IGNORE);
        MPI_Recv(&b, 1, MPI_DOUBLE, 0, 0, MPI_COMM_WORLD, MPI_STATUS_IGNORE);
        MPI_Recv(&n, 1, MPI_INT,    0, 0, MPI_COMM_WORLD, MPI_STATUS_IGNORE);
    }

    h = (b - a) / n;

    /* (a) Balanceamento de carga: primeiros (n % p) ranks ficam com
     *     ceil(n/p) trapézios, os demais com floor(n/p). */
    int rem      = n % comm_sz;
    int local_n  = n / comm_sz + (my_rank < rem ? 1 : 0);
    int offset   = my_rank * (n / comm_sz) + (my_rank < rem ? my_rank : rem);
    double local_a = a + offset * h;
    double local_b = local_a + local_n * h;

    /* (b) Timing com MPI_Wtime */
    MPI_Barrier(MPI_COMM_WORLD);
    double start = MPI_Wtime();

    double local_integral = 0.0;
    if (local_n > 0) {
        local_integral = Trap(local_a, local_b, local_n);
    }

    double finish = MPI_Wtime();
    double local_elapsed = finish - start;
    double elapsed;
    MPI_Reduce(&local_elapsed, &elapsed, 1, MPI_DOUBLE,
               MPI_MAX, 0, MPI_COMM_WORLD);

    if (my_rank != 0) {
        MPI_Send(&local_integral, 1, MPI_DOUBLE, 0, 0, MPI_COMM_WORLD);
    } else {
        double total_integral = local_integral;
        for (int src = 1; src < comm_sz; src++) {
            MPI_Recv(&local_integral, 1, MPI_DOUBLE, src, 0, MPI_COMM_WORLD, MPI_STATUS_IGNORE);
            total_integral += local_integral;
        }

        printf("RESULT: p=%d n=%d integral=%.15e Tp=%.6f\n",
               comm_sz, n, total_integral, elapsed);

        /* Para verificação rápida */
        double exact = 2.0;  /* integral de sin(x) em [0, pi] */
        double error = fabs(total_integral - exact);
        printf("  Exact = %.15e, Error = %.4e, h = %.4e\n",
               exact, error, h);
    }

    MPI_Finalize();
    return 0;
}
