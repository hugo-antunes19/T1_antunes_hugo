/* student_three_bcasts.c
 * Exercício 6 — Versão com três MPI_Bcast separados (sem tipo derivado).
 *
 * Rank 0 preenche um Student e transmite campo a campo:
 *   1) MPI_Bcast para name  (50 chars)
 *   2) MPI_Bcast para grade (1 double)
 *   3) MPI_Bcast para id    (1 int)
 * Total: 3 chamadas MPI_Bcast.
 *
 * Compare com student_struct.c que usa apenas 1 MPI_Bcast.
 *
 * Compile: mpicc -O2 -Wall -o student_three_bcasts student_three_bcasts.c
 * Run:     mpiexec -n 4 ./student_three_bcasts
 */

#include <stdio.h>
#include <string.h>
#include <mpi.h>

struct Student {
    char   name[50];
    double grade;
    int    id;
};

int main(void) {
    int my_rank;
    struct Student s;

    MPI_Init(NULL, NULL);
    MPI_Comm_rank(MPI_COMM_WORLD, &my_rank);

    if (my_rank == 0) {
        strncpy(s.name, "Hugo Leandro Antunes", 50);
        s.grade = 9.5;
        s.id    = 123143543;
        printf("[Rank 0] Enviando Student via 3 MPI_Bcast separados:\n");
        printf("  name  = \"%s\"\n", s.name);
        printf("  grade = %.1f\n", s.grade);
        printf("  id    = %d\n\n", s.id);
    }

    /* Três chamadas separadas de MPI_Bcast — uma por campo */
    MPI_Bcast(s.name,  50, MPI_CHAR,   0, MPI_COMM_WORLD);  /* Bcast 1 */
    MPI_Bcast(&s.grade, 1, MPI_DOUBLE, 0, MPI_COMM_WORLD);  /* Bcast 2 */
    MPI_Bcast(&s.id,    1, MPI_INT,    0, MPI_COMM_WORLD);  /* Bcast 3 */

    if (my_rank != 0) {
        printf("[Rank %d] Recebido via 3 Bcasts:\n", my_rank);
        printf("  name  = \"%s\"\n", s.name);
        printf("  grade = %.1f\n", s.grade);
        printf("  id    = %d\n", s.id);
    }

    MPI_Finalize();
    return 0;
}
