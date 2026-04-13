#include <mpi.h>

#include <iostream>

using namespace std;

int main(int argc, char* argv[]) {
    MPI_Init(&argc, &argv);

    int rank = 0;
    int comm = 0;
    MPI_Comm_rank(MPI_COMM_WORLD, &rank);
    MPI_Comm_size(MPI_COMM_WORLD, &comm);

    long long suma_local = static_cast<long long>(rank + 1);
    long long suma_global = suma_local;

    for (int paso = 1; paso < comm; paso *= 2) {
        const int bloque = 2 * paso;

        if (rank % bloque == paso) {
            MPI_Send(&suma_global, 1, MPI_LONG_LONG, rank - paso, 0, MPI_COMM_WORLD);
            break;
        }

        if (rank % bloque == 0 && rank + paso < comm) {
            long long suma_recibida = 0;
            MPI_Recv(&suma_recibida, 1, MPI_LONG_LONG, rank + paso, 0,
                     MPI_COMM_WORLD, MPI_STATUS_IGNORE);
            suma_global += suma_recibida;
        }
    }

    if (rank == 0) {
        cout << "Suma global: " << suma_global << '\n';
    }

    MPI_Finalize();
    return 0;
}