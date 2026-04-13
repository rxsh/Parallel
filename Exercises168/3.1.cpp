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

    const int cantidad_intervalos = 5;
    const double valor_minimo = 0.0;
    const double valor_maximo = 5.0;
    const double ancho_intervalo = (valor_maximo - valor_minimo) / cantidad_intervalos;

    vector<double> datos;
    int cantidad_datos = 0;

    if (rank == 0) {
        datos = {
            1.3, 2.9, 0.4, 0.3, 1.3,
            4.4, 1.7, 0.4, 3.2, 0.3,
            4.9, 2.4, 3.1, 4.4, 3.9,
            0.4, 4.2, 4.5, 4.9, 0.9
        };
        cantidad_datos = static_cast<int>(datos.size());
    }

    MPI_Bcast(&cantidad_datos, 1, MPI_INT, 0, MPI_COMM_WORLD);

    vector<int> conteos_envio(comm, 0);
    vector<int> desplazamientos(comm, 0);

    if (rank == 0) {
        const int base = cantidad_datos / comm;
        const int extra = cantidad_datos % comm;

        for (int i = 0; i < comm; i++) {
            conteos_envio[i] = base + (i < extra ? 1 : 0);
            if (i > 0) {
                desplazamientos[i] = desplazamientos[i - 1] + conteos_envio[i - 1];
            }
        }
    }

    int cantidad_local = 0;
    MPI_Scatter(conteos_envio.data(), 1, MPI_INT, &cantidad_local, 1, MPI_INT, 0,
                MPI_COMM_WORLD);

    vector<double> datos_locales(cantidad_local);
    MPI_Scatterv(rank == 0 ? datos.data() : nullptr, conteos_envio.data(),
                 desplazamientos.data(), MPI_DOUBLE, datos_locales.data(), cantidad_local,
                 MPI_DOUBLE, 0, MPI_COMM_WORLD);

    vector<int> hist_local(cantidad_intervalos, 0);
    for (double valor : datos_locales) {
        if (valor >= valor_minimo && valor <= valor_maximo) {
            int intervalo = static_cast<int>((valor - valor_minimo) / ancho_intervalo);
            if (intervalo == cantidad_intervalos) {
                intervalo = cantidad_intervalos - 1;
            }
            hist_local[intervalo]++;
        }
    }

    vector<int> hist_global(cantidad_intervalos, 0);
    MPI_Reduce(hist_local.data(), hist_global.data(), cantidad_intervalos, MPI_INT,
               MPI_SUM, 0, MPI_COMM_WORLD);

    if (rank == 0) {
        cout << "Histograma para valores en [0.0, 5.0]:\n";
        for (int i = 0; i < cantidad_intervalos; i++) {
            const double intervalo_min = valor_minimo + i * ancho_intervalo;
            const double intervalo_max = intervalo_min + ancho_intervalo;

            cout << fixed << setprecision(1);
            if (i == cantidad_intervalos - 1) {
                cout << "[" << intervalo_min << ", " << intervalo_max << "]";
            } else {
                cout << "[" << intervalo_min << ", " << intervalo_max << ")";
            }
            cout << ": " << hist_global[i] << '\n';
        }
    }

    MPI_Finalize();
    return 0;
}
