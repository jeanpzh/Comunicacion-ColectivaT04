#include <mpi.h>
#include <stdio.h>
#include <stdlib.h>

#define N 4  // Tamaño de la matriz N x N

int main(int argc, char** argv) {
    int rank;
    MPI_Init(&argc, &argv);
    MPI_Comm_rank(MPI_COMM_WORLD, &rank);

    int matrix[N * N];
    int count = N; // Cantidad de bloques = cantidad de filas
    int blocklengths[N];
    int displacements[N];

    // Inicializar bloques para triangular superior
    for (int i = 0; i < N; i++) {
        blocklengths[i] = N - i;            // Disminuye en cada fila
        displacements[i] = i * N + i;       // Comienza desde la diagonal
    }

    // Crear tipo derivado
    MPI_Datatype upper_triangle_T;
    MPI_Type_indexed(count, blocklengths, displacements, MPI_INT, &upper_triangle_T);
    MPI_Type_commit(&upper_triangle_T);

    if (rank == 0) {
        // Inicializar matriz 1D como ejemplo
        for (int i = 0; i < N * N; i++)
            matrix[i] = i;

        // Enviar solo la parte triangular superior
        MPI_Send(matrix, 1, upper_triangle_T, 1, 0, MPI_COMM_WORLD);
    }

    else if (rank == 1) {
        // Buffer para recibir (tamaño máximo N*N por simplicidad)
        int received[N * N] = {0}; // Inicializa todo en 0

        MPI_Recv(received, 1, upper_triangle_T, 0, 0, MPI_COMM_WORLD, MPI_STATUS_IGNORE);

        printf("Proceso 1 recibió:\n");
        for (int i = 0; i < N * N; i++) {
            printf("%2d ", received[i]);
            if ((i + 1) % N == 0) printf("\n");
        }
    }

    MPI_Type_free(&upper_triangle_T);
    MPI_Finalize();
    return 0;
}
