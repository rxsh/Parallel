#include <mpi.h>

#include <iomanip>
#include <iostream>
#include <vector>

using namespace std;

int main(int argc, char* argv[]) {
    MPI_Init(&argc, &argv);

    int rank = 0;
    int comm_sz = 0;
    MPI_Comm_rank(MPI_COMM_WORLD, &rank);
    MPI_Comm_size(MPI_COMM_WORLD, &comm_sz);

    const int bin_count = 5;
    const double min_value = 0.0;
    const double max_value = 5.0;
    const double bin_width = (max_value - min_value) / bin_count;

    std::vector<double> data;
    int n = 0;

    if (rank == 0) {
        data = {
            1.3, 2.9, 0.4, 0.3, 1.3,
            4.4, 1.7, 0.4, 3.2, 0.3,
            4.9, 2.4, 3.1, 4.4, 3.9,
            0.4, 4.2, 4.5, 4.9, 0.9
        };
        n = static_cast<int>(data.size());
    }

    MPI_Bcast(&n, 1, MPI_INT, 0, MPI_COMM_WORLD);

    vector<int> send_counts(comm_sz, 0);
    vector<int> displs(comm_sz, 0);

    if (rank == 0) {
        const int base = n / comm_sz;
        const int extra = n % comm_sz;

        for (int i = 0; i < comm_sz; i++) {
            send_counts[i] = base + (i < extra ? 1 : 0);
            if (i > 0) {
                displs[i] = displs[i - 1] + send_counts[i - 1];
            }
        }
    }

    int local_n = 0;
    MPI_Scatter(send_counts.data(), 1, MPI_INT, &local_n, 1, MPI_INT, 0,
                MPI_COMM_WORLD);

    vector<double> local_data(local_n);
    MPI_Scatterv(rank == 0 ? data.data() : nullptr, send_counts.data(),
                 displs.data(), MPI_DOUBLE, local_data.data(), local_n,
                 MPI_DOUBLE, 0, MPI_COMM_WORLD);

    vector<int> local_hist(bin_count, 0);
    for (double value : local_data) {
        if (value >= min_value && value <= max_value) {
            int bin = static_cast<int>((value - min_value) / bin_width);
            if (bin == bin_count) {
                bin = bin_count - 1;
            }
            local_hist[bin]++;
        }
    }

    vector<int> global_hist(bin_count, 0);
    MPI_Reduce(local_hist.data(), global_hist.data(), bin_count, MPI_INT,
               MPI_SUM, 0, MPI_COMM_WORLD);

    if (rank == 0) {
        cout << "Histograma para valores en [0.0, 5.0]:\n";
        for (int i = 0; i < bin_count; i++) {
            const double bin_min = min_value + i * bin_width;
            const double bin_max = bin_min + bin_width;

            cout << fixed << setprecision(1);
            if (i == bin_count - 1) {
                cout << "[" << bin_min << ", " << bin_max << "]";
            } else {
                cout << "[" << bin_min << ", " << bin_max << ")";
            }
            cout << ": " << global_hist[i] << '\n';
        }
    }

    MPI_Finalize();
    return 0;
}
