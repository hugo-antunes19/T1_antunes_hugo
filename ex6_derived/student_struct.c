/* student_struct.c
 * Exercício 6 — Tipo de dado derivado MPI para struct Student.
 *
 * Define um tipo derivado MPI que descreve:
 *   struct Student { char name[50]; double grade; int id; };
 * Rank 0 preenche um Student e transmite com UMA chamada de MPI_Bcast.
 * Todos os outros ranks imprimem o registro recebido.
 *
 * Usa offsetof de <stddef.h> para os deslocamentos (padding-safe).
 *
 * Compile: mpicc -O2 -Wall -o student_struct student_struct.c
 * Run:     mpiexec -n 4 ./student_struct
 */

#include <stdio.h>
#include <string.h>
#include <stddef.h>     /* offsetof */
#include <mpi.h>

struct Student {
    char   name[50];
    double grade;
    int    id;
};

static MPI_Datatype build_student_type(void) {
    MPI_Datatype student_type;
    int          block_lengths[3] = {50, 1, 1};
    MPI_Aint     displacements[3];
    MPI_Datatype types[3] = {MPI_CHAR, MPI_DOUBLE, MPI_INT};

    displacements[0] = offsetof(struct Student, name);
    displacements[1] = offsetof(struct Student, grade);
    displacements[2] = offsetof(struct Student, id);

    MPI_Type_create_struct(3, block_lengths, displacements,
                           types, &student_type);

    /* Resize para considerar o tamanho real da struct (com padding final) */
    MPI_Datatype resized;
    MPI_Type_create_resized(student_type, 0, sizeof(struct Student), &resized);
    MPI_Type_commit(&resized);
    MPI_Type_free(&student_type);

    return resized;
}

int main(void) {
    int my_rank;
    struct Student s;

    MPI_Init(NULL, NULL);
    MPI_Comm_rank(MPI_COMM_WORLD, &my_rank);

    MPI_Datatype student_type = build_student_type();

    if (my_rank == 0) {
        strncpy(s.name, "Hugo Leandro Antunes", 50);
        s.grade = 9.5;
        s.id    = 123143543;
        printf("[Rank 0] Enviando Student via tipo derivado (1 MPI_Bcast):\n");
        printf("  name  = \"%s\"\n", s.name);
        printf("  grade = %.1f\n", s.grade);
        printf("  id    = %d\n\n", s.id);
    }

    /* Uma única chamada de MPI_Bcast */
    MPI_Bcast(&s, 1, student_type, 0, MPI_COMM_WORLD);

    if (my_rank != 0) {
        printf("[Rank %d] Recebido via tipo derivado:\n", my_rank);
        printf("  name  = \"%s\"\n", s.name);
        printf("  grade = %.1f\n", s.grade);
        printf("  id    = %d\n", s.id);
    }

    MPI_Type_free(&student_type);
    MPI_Finalize();
    return 0;
}
