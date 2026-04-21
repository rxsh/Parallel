#include <mpi.h>

#include <iomanip>
#include <iostream>
#include <vector>

using namespace std;

int main(int argc, char* argv[]) {
    MPI_Init(&argc, &argv);

    int rank = 0;
    int comm = 0;
    MPI_Comm_rank(MPI_COMM_WORLD, &rank);
    MPI_Comm_size(MPI_COMM_WORLD, &comm);

    int orden = 0;
    vector<double> matriz;
    vector<double> vector_entrada;

    if (rank == 0) {
        cout << "Ingrese el orden n de la matriz cuadrada: ";
        cin >> orden;

        matriz.resize(static_cast<size_t>(orden) * static_cast<size_t>(orden));
        vector_entrada.resize(orden);

        cout << "Ingrese los elementos de la matriz por filas:\n";
        for (int i = 0; i < orden; i++) {
            for (int j = 0; j < orden; j++) {
                cin >> matriz[i * orden + j];
            }
        }

        cout << "Ingrese el vector de entrada:\n";
        for (int i = 0; i < orden; i++) {
            cin >> vector_entrada[i];
        }
    }

    MPI_Bcast(&orden, 1, MPI_INT, 0, MPI_COMM_WORLD);

    const int columnas_locales = orden / comm;
    vector<double> matriz_local(static_cast<size_t>(orden) * columnas_locales);
    vector<double> vector_local(columnas_locales);

    if (rank == 0) {
        for (int proceso = 0; proceso < comm; proceso++) {
            const int columna_inicial = proceso * columnas_locales;

            if (proceso == 0) {
                for (int j = 0; j < columnas_locales; j++) {
                    vector_local[j] = vector_entrada[columna_inicial + j];
                }

                for (int fila = 0; fila < orden; fila++) {
                    for (int j = 0; j < columnas_locales; j++) {
                        matriz_local[fila * columnas_locales + j] =
                            matriz[fila * orden + columna_inicial + j];
                    }
                }
            } else {
                MPI_Send(&vector_entrada[columna_inicial], columnas_locales, MPI_DOUBLE,
                         proceso, 0, MPI_COMM_WORLD);

                for (int fila = 0; fila < orden; fila++) {
                    MPI_Send(&matriz[fila * orden + columna_inicial], columnas_locales,
                             MPI_DOUBLE, proceso, 1, MPI_COMM_WORLD);
                }
            }
        }
    } else {
        MPI_Recv(vector_local.data(), columnas_locales, MPI_DOUBLE, 0, 0,
                 MPI_COMM_WORLD, MPI_STATUS_IGNORE);

        for (int fila = 0; fila < orden; fila++) {
            MPI_Recv(&matriz_local[fila * columnas_locales], columnas_locales,
                     MPI_DOUBLE, 0, 1, MPI_COMM_WORLD, MPI_STATUS_IGNORE);
        }
    }

    vector<double> resultado_local(orden, 0.0);
    for (int fila = 0; fila < orden; fila++) {
        for (int j = 0; j < columnas_locales; j++) {
            resultado_local[fila] += matriz_local[fila * columnas_locales + j] *
                                     vector_local[j];
        }
    }

    vector<double> resultado_global(orden, 0.0);
    MPI_Reduce(resultado_local.data(), resultado_global.data(), orden, MPI_DOUBLE,
               MPI_SUM, 0, MPI_COMM_WORLD);

    if (rank == 0) {
        cout << fixed << setprecision(6);
        cout << "Resultado y = A*x:\n";
        for (int fila = 0; fila < orden; fila++) {
            cout << resultado_global[fila] << '\n';
        }
    }

    MPI_Finalize();
    return 0;
}