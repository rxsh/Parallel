#include <chrono>
#include <cstdlib>
#include <iomanip>
#include <iostream>
#include <vector>

using namespace std;
using reloj = chrono::high_resolution_clock;

static void llenar_datos(vector<double>& A, vector<double>& x, int n) {
    for (int i = 0; i < n; i++) {
        x[i] = 1.0 + (i % 7);
        for (int j = 0; j < n; j++) {
            A[(size_t)i * n + j] = (double)((i + j) % 100) / 25.0;
        }
    }
}

static double ejecutar_orden_ij(const vector<double>& A, const vector<double>& x, vector<double>& y, int n) {
    fill(y.begin(), y.end(), 0.0);

    auto inicio = reloj::now();
    for (int i = 0; i < n; i++) {
        for (int j = 0; j < n; j++) {
            y[i] += A[(size_t)i * n + j] * x[j];
        }
    }
    auto fin = reloj::now();

    return chrono::duration<double, milli>(fin - inicio).count();
}

static double ejecutar_orden_ji(const vector<double>& A, const vector<double>& x, vector<double>& y, int n) {
    fill(y.begin(), y.end(), 0.0);

    auto inicio = reloj::now();
    for (int j = 0; j < n; j++) {
        for (int i = 0; i < n; i++) {
            y[i] += A[(size_t)i * n + j] * x[j];
        }
    }
    auto fin = reloj::now();

    return chrono::duration<double, milli>(fin - inicio).count();
}

static double checksum(const vector<double>& y) {
    double suma = 0.0;
    for (double valor : y) {
        suma += valor;
    }
    return suma;
}

int main(int argc, char* argv[]) {
    vector<int> tamanos;

    if (argc > 1) {
        for (int i = 1; i < argc; i++) {
            tamanos.push_back(atoi(argv[i]));
        }
    } else {
        tamanos = {256, 512, 1024};
    }

    cout << "Comparacion de dos bucles anidados para y = A*x" << '\n';
    cout << left << setw(10) << "N"
         << setw(18) << "orden i-j (ms)"
         << setw(18) << "orden j-i (ms)"
         << setw(14) << "razon"
         << "checksum" << '\n';

    for (int n : tamanos) {
        vector<double> A((size_t)n * n);
        vector<double> x(n);
        vector<double> y(n);

        llenar_datos(A, x, n);

        double tiempo_ij = ejecutar_orden_ij(A, x, y, n);
        double control = checksum(y);

        double tiempo_ji = ejecutar_orden_ji(A, x, y, n);
        control += checksum(y);

        cout << left << setw(10) << n
             << setw(18) << fixed << setprecision(3) << tiempo_ij
             << setw(18) << fixed << setprecision(3) << tiempo_ji
             << setw(14) << fixed << setprecision(2) << (tiempo_ji / tiempo_ij)
             << fixed << setprecision(2) << control << '\n';
    }

    return 0;
}
