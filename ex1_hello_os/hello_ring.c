/* hello_ring.c
 * Exercício 1(b) — Hello World em anel.
 *
 * Cada rank k envia sua saudação ao rank (k+1) % p e recebe do rank
 * (k-1+p) % p.  Usa MPI_Sendrecv para evitar deadlock.
 * O processo 0 coleta e imprime todas as saudações.
 *
 * MPI_Sendrecv realiza envio e recebimento de forma
 * atômica, eliminando a possibilidade de deadlock circular que
 * ocorreria com chamadas separadas de MPI_Send seguido de MPI_Recv
 * (todos os ranks ficariam bloqueados esperando receber antes de
 * enviar).
 *
 * Compile: mpicc -O2 -Wall -o hello_ring hello_ring.c
 * Run:     mpiexec -n 4 ./hello_ring
 */

#include <stdio.h>
#include <string.h>
#include <mpi.h>

#define MAX_STRING 200

int main(void) {
    char send_buf[MAX_STRING];
    char recv_buf[MAX_STRING];
    int  comm_sz, my_rank;

    MPI_Init(NULL, NULL);
    MPI_Comm_size(MPI_COMM_WORLD, &comm_sz);
    MPI_Comm_rank(MPI_COMM_WORLD, &my_rank);

    /* Preparar a saudação deste rank */
    snprintf(send_buf, MAX_STRING,
             "Greetings from rank %d of %d!", my_rank, comm_sz);

    int dest = (my_rank + 1) % comm_sz;
    int src  = (my_rank - 1 + comm_sz) % comm_sz;

    /* MPI_Sendrecv: enviar ao vizinho da direita e receber do vizinho
     * da esquerda em uma única chamada atômica — sem deadlock. */
    MPI_Sendrecv(send_buf, (int)strlen(send_buf) + 1, MPI_CHAR, dest, 0,
                 recv_buf, MAX_STRING, MPI_CHAR, src, 0,
                 MPI_COMM_WORLD, MPI_STATUS_IGNORE);

    /* Após a rotação, cada rank possui a saudação do seu vizinho à
     * esquerda.  Agora, fazemos p-1 rotações adicionais para que
     * o rank 0 colete todas as mensagens na ordem. */

    /* Abordagem simples: rank 0 imprime o que recebeu do anel,
     * e os demais ranks enviam ao rank 0 o que receberam. */
    if (my_rank == 0) {
        char collect_buf[MAX_STRING];
        char final_msg[MAX_STRING];
        snprintf(final_msg, MAX_STRING, "Rank %d received from Rank %d: '%s'", my_rank, src, recv_buf);

        /* Coletar dos ranks 1..p-1 na ordem */
        char all_greetings[comm_sz][MAX_STRING];
        /* Copiar a do rank 0 */
        strncpy(all_greetings[0], final_msg, MAX_STRING);

        for (int q = 1; q < comm_sz; q++) {
            MPI_Recv(collect_buf, MAX_STRING, MPI_CHAR,
                     q, 1, MPI_COMM_WORLD, MPI_STATUS_IGNORE);
            strncpy(all_greetings[q], collect_buf, MAX_STRING);
        }

        /* Imprimir na ordem dos ranks */
        for (int q = 0; q < comm_sz; q++) {
            printf("%s\n", all_greetings[q]);
        }
    } else {
        /* Enviar a mensagem que relata o que recebemos ao rank 0 para coleta */
        char final_msg[MAX_STRING];
        snprintf(final_msg, MAX_STRING, "Rank %d received from Rank %d: '%s'", my_rank, src, recv_buf);
        MPI_Send(final_msg, (int)strlen(final_msg) + 1, MPI_CHAR,
                 0, 1, MPI_COMM_WORLD);
    }

    MPI_Finalize();
    return 0;
}
