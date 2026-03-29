#include <chrono>
#include <iostream>
#include <vector>

using namespace std;

int main() {
    const int n = 10'000'000;
    vector<int> datos(n, 1);

    long long suma_dependiente = 0;
    auto inicio1 = chrono::high_resolution_clock::now();

    for (int i = 0; i < n; ++i) {
        suma_dependiente += datos[i];
    }

    auto fin1 = chrono::high_resolution_clock::now();

    long long suma1 = 0;
    long long suma2 = 0;
    auto inicio2 = chrono::high_resolution_clock::now();

    for (int i = 0; i < n; i += 2) {
        suma1 += datos[i];
        if (i + 1 < n) {
            suma2 += datos[i + 1];
        }
    }

    auto fin2 = chrono::high_resolution_clock::now();

    auto tiempo1 = chrono::duration_cast<chrono::microseconds>(fin1 - inicio1);
    auto tiempo2 = chrono::duration_cast<chrono::microseconds>(fin2 - inicio2);

    cout << "=== Paralelismo a nivel de instruccion (ILP) ===\n\n";

    cout << "Caso 1: instrucciones dependientes\n";
    cout << "Resultado: " << suma_dependiente << "\n";
    cout << "Tiempo: " << tiempo1.count() << " microsegundos\n\n";

    cout << "Caso 2: instrucciones mas independientes\n";
    cout << "Resultado: " << (suma1 + suma2) << "\n";
    cout << "Tiempo: " << tiempo2.count() << " microsegundos\n\n";

    return 0;
}