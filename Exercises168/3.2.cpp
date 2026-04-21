#include <mpi.h>

#include <chrono>
#include <iomanip>
#include <iostream>
#include <random>

using namespace std;

int main(int argc, char* argv[]) {
    MPI_Init(&argc, &argv);

    int rank = 0;
    int cantidad_procesos = 0;
    MPI_Comm_rank(MPI_COMM_WORLD, &rank);
    MPI_Comm_size(MPI_COMM_WORLD, &cantidad_procesos);

    long long total_lanzamientos = 0;

    if (rank == 0) {
        cout << "Ingrese el numero total de lanzamientos: ";
        cin >> total_lanzamientos;

        if (!cin || total_lanzamientos < 0) {
            cerr << "Entrada invalida. El numero de lanzamientos debe ser un entero no negativo.\n";
            total_lanzamientos = 0;
        }
    }

    MPI_Bcast(&total_lanzamientos, 1, MPI_LONG_LONG, 0, MPI_COMM_WORLD);

    const long long base = total_lanzamientos / cantidad_procesos;
    const long long extra = total_lanzamientos % cantidad_procesos;
    const long long lanzamientos_locales = base + (rank < extra ? 1 : 0);

    const auto marca_tiempo =
        chrono::high_resolution_clock::now().time_since_epoch().count();
    mt19937_64 generador(static_cast<unsigned long long>(marca_tiempo) + static_cast<unsigned long long>(rank) * 0x9E3779B97F4A7C15ULL);
    uniform_real_distribution<double> distribucion(-1.0, 1.0);

    long long en_circulo_local = 0;
    for (long long lanzamiento = 0; lanzamiento < lanzamientos_locales; lanzamiento++) {
        const double x = distribucion(generador);
        const double y = distribucion(generador);
        const double distancia_cuadrada = x * x + y * y;

        if (distancia_cuadrada <= 1.0) {
            en_circulo_local++;
        }
    }

    long long en_circulo_global = 0;
    MPI_Reduce(&en_circulo_local, &en_circulo_global, 1, MPI_LONG_LONG, MPI_SUM,
               0, MPI_COMM_WORLD);

    if (rank == 0) {
        const double estimacion_pi =
            (total_lanzamientos > 0)
                ? 4.0 * static_cast<double>(en_circulo_global) /
                      static_cast<double>(total_lanzamientos)
                : 0.0;

        cout << fixed << setprecision(10);
        cout << "Lanzamientos totales: " << total_lanzamientos << '\n';
        cout << "Puntos dentro del circulo: " << en_circulo_global << '\n';
        cout << "Estimacion de pi: " << estimacion_pi << '\n';
    }

    MPI_Finalize();
    return 0;
}
