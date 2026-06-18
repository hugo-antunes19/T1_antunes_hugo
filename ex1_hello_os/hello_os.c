/* hello_os.c
 * Exercício 1(a) — Introspecção em nível de SO.
 *
 * Cada processo MPI reporta seu rank, PID e núcleo de CPU.
 * Processo 0 coleta e imprime todas as saudações.
 *
 * Compile: mpicc -O2 -Wall -o hello_os hello_os.c
 * Run:     mpiexec -n 8 ./hello_os
 */

#define _GNU_SOURCE
#include <stdio.h>
#include <string.h>
#include <unistd.h>    /* getpid() */
#include <sched.h>     /* sched_getcpu() */
#include <mpi.h>

#define MAX_STRING 200

int main(void) {
    char greeting[MAX_STRING];
    int  comm_sz, my_rank;

    MPI_Init(NULL, NULL);
    MPI_Comm_size(MPI_COMM_WORLD, &comm_sz);
    MPI_Comm_rank(MPI_COMM_WORLD, &my_rank);

    int pid = (int) getpid();
    int cpu = sched_getcpu();

    snprintf(greeting, MAX_STRING,
             "Hello from rank %d/%d -- PID = %d, CPU = %d",
             my_rank, comm_sz, pid, cpu);

    if (my_rank != 0) {
        MPI_Send(greeting, (int)strlen(greeting) + 1, MPI_CHAR,
                 0, 0, MPI_COMM_WORLD);
    } else {
        printf("%s\n", greeting);
        for (int q = 1; q < comm_sz; q++) {
            MPI_Recv(greeting, MAX_STRING, MPI_CHAR,
                     q, 0, MPI_COMM_WORLD, MPI_STATUS_IGNORE);
            printf("%s\n", greeting);
        }
    }

    MPI_Finalize();
    return 0;
}
