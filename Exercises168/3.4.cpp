#include <mpi.h>

#include <iostream>

using namespace std;

static int potencia_2(int valor) {
    int potencia = 1;
    while (potencia * 2 <= valor) {
        potencia *= 2;
    }
    return potencia;
}

int main(int argc, char* argv[]) {
    MPI_Init(&argc, &argv);

    int rank = 0;
    int comm = 0;
    MPI_Comm_rank(MPI_COMM_WORLD, &rank);
    MPI_Comm_size(MPI_COMM_WORLD, &comm);

    long long valor_local = static_cast<long long>(rank + 1);
    long long suma_local = valor_local;

    int potencia = potencia_2(comm);

    if (comm == potencia) {
        for (int mascara = 1; mascara < comm; mascara <<= 1) {
            int pareja = rank ^ mascara;

            if (rank < pareja) {
                long long recibido = 0;
                MPI_Recv(&recibido, 1, MPI_LONG_LONG, pareja, 0,
                         MPI_COMM_WORLD, MPI_STATUS_IGNORE);
                suma_local += recibido;
            } else {
                MPI_Send(&suma_local, 1, MPI_LONG_LONG, pareja, 0, MPI_COMM_WORLD);
                break;
            }
        }
    } else {
        long long suma_global = suma_local;

        if (rank >= potencia) {
            int destino = rank - potencia;
            MPI_Send(&suma_global, 1, MPI_LONG_LONG, destino, 0, MPI_COMM_WORLD);
        } else {
            if (rank + potencia < comm) {
                long long recibido = 0;
                MPI_Recv(&recibido, 1, MPI_LONG_LONG, rank + potencia, 0,
                         MPI_COMM_WORLD, MPI_STATUS_IGNORE);
                suma_global += recibido;
            }

            for (int mascara = potencia / 2; mascara >= 1; mascara >>= 1) {
                int pareja = rank ^ mascara;

                if (rank < pareja) {
                    long long recibido = 0;
                    MPI_Recv(&recibido, 1, MPI_LONG_LONG, pareja, 0,
                             MPI_COMM_WORLD, MPI_STATUS_IGNORE);
                    suma_global += recibido;
                } else {
                    MPI_Send(&suma_global, 1, MPI_LONG_LONG, pareja, 0,
                             MPI_COMM_WORLD);
                    break;
                }
            }
        }

        suma_local = suma_global;
    }

    if (rank == 0) {
        cout << "Suma global: " << suma_local << '\n';
    }

    MPI_Finalize();
    return 0;
}