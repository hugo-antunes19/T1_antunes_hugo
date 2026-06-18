/* hello_gather.c
 * Exercício 4(a) — Hello World com MPI_Gather.
 *
 * Cada rank prepara uma saudação em um buffer de tamanho fixo.
 * O rank 0 coleta todas as saudações com MPI_Gather (em vez de
 * um laço de MPI_Recv) e imprime na ordem dos ranks.
 *
 * Compile: mpicc -O2 -Wall -o hello_gather hello_gather.c
 * Run:     mpiexec -n 4 ./hello_gather
 */

#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <mpi.h>

#define MAX_STRING 100

int main(void) {
    int  comm_sz, my_rank;
    char greeting[MAX_STRING];

    MPI_Init(NULL, NULL);
    MPI_Comm_size(MPI_COMM_WORLD, &comm_sz);
    MPI_Comm_rank(MPI_COMM_WORLD, &my_rank);

    /* Cada rank prepara sua saudação */
    snprintf(greeting, MAX_STRING,
             "Greetings from process %d of %d!", my_rank, comm_sz);

    /* Rank 0 aloca buffer para todas as saudações */
    char *all_greetings = NULL;
    if (my_rank == 0) {
        all_greetings = (char *) malloc(comm_sz * MAX_STRING * sizeof(char));
    }

    /* Coletar todas as saudações com MPI_Gather */
    MPI_Gather(greeting,      MAX_STRING, MPI_CHAR,
               all_greetings, MAX_STRING, MPI_CHAR,
               0, MPI_COMM_WORLD);

    /* Rank 0 imprime na ordem dos ranks */
    if (my_rank == 0) {
        for (int q = 0; q < comm_sz; q++) {
            printf("%s\n", all_greetings + q * MAX_STRING);
        }
        free(all_greetings);
    }

    MPI_Finalize();
    return 0;
}
