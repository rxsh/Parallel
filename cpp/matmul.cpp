#include <algorithm>
#include <chrono>
#include <cstdlib>
#include <iomanip>
#include <iostream>
#include <vector>

using namespace std;
using reloj = chrono::high_resolution_clock;

static void llenar_matrices(vector<double>& A, vector<double>& B, int n) {
    for (int i = 0; i < n; i++) {
        for (int j = 0; j < n; j++) {
            A[(size_t)i * n + j] = (double)((i + 2 * j) % 100) / 20.0;
            B[(size_t)i * n + j] = (double)((3 * i + j) % 100) / 30.0;
        }
    }
}

static void limpiar(vector<double>& C) {
    fill(C.begin(), C.end(), 0.0);
}

static double multiplicacion_clasica(const vector<double>& A, const vector<double>& B, vector<double>& C, int n) {
    limpiar(C);

    auto inicio = reloj::now();
    for (int i = 0; i < n; i++) {
        for (int j = 0; j < n; j++) {
            double suma = 0.0;
            for (int k = 0; k < n; k++) {
                suma += A[(size_t)i * n + k] * B[(size_t)k * n + j];
            }
            C[(size_t)i * n + j] = suma;
        }
    }
    auto fin = reloj::now();

    return chrono::duration<double, milli>(fin - inicio).count();
}

static double multiplicacion_bloques(const vector<double>& A, const vector<double>& B, vector<double>& C, int n, int bloque) {
    limpiar(C);

    auto inicio = reloj::now();
    for (int ii = 0; ii < n; ii += bloque) {
        for (int jj = 0; jj < n; jj += bloque) {
            for (int kk = 0; kk < n; kk += bloque) {
                int fin_i = min(ii + bloque, n);
                int fin_j = min(jj + bloque, n);
                int fin_k = min(kk + bloque, n);

                for (int i = ii; i < fin_i; i++) {
                    for (int j = jj; j < fin_j; j++) {
                        double suma = C[(size_t)i * n + j];
                        for (int k = kk; k < fin_k; k++) {
                            suma += A[(size_t)i * n + k] * B[(size_t)k * n + j];
                        }
                        C[(size_t)i * n + j] = suma;
                    }
                }
            }
        }
    }
    auto fin = reloj::now();

    return chrono::duration<double, milli>(fin - inicio).count();
}

static double checksum(const vector<double>& C) {
    double suma = 0.0;
    for (double valor : C) {
        suma += valor;
    }
    return suma;
}

int main(int argc, char* argv[]) {
    vector<int> tamanos;
    int bloque = 32;

    if (argc > 1) {
        for (int i = 1; i < argc; i++) {
            tamanos.push_back(atoi(argv[i]));
        }
    } else {
        tamanos = {64, 128, 256, 384};
    }

    cout << "Comparacion de multiplicacion de matrices" << '\n';
    cout << "Bloque usado en la version por bloques: " << bloque << '\n';
    cout << left << setw(10) << "N"
         << setw(20) << "clasica (ms)"
         << setw(20) << "bloques (ms)"
         << setw(14) << "razon"
         << "checksum" << '\n';

    for (int n : tamanos) {
        vector<double> A((size_t)n * n);
        vector<double> B((size_t)n * n);
        vector<double> C((size_t)n * n);

        llenar_matrices(A, B, n);

        double tiempo_clasico = multiplicacion_clasica(A, B, C, n);
        double control = checksum(C);

        double tiempo_bloques = multiplicacion_bloques(A, B, C, n, bloque);
        control += checksum(C);

        cout << left << setw(10) << n
             << setw(20) << fixed << setprecision(3) << tiempo_clasico
             << setw(20) << fixed << setprecision(3) << tiempo_bloques
             << setw(14) << fixed << setprecision(2) << (tiempo_clasico / tiempo_bloques)
             << fixed << setprecision(2) << control << '\n';
    }

    return 0;
}
