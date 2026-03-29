#include <iostream>
#include <vector>

using namespace std;

int main() {
    const int tam_pagina_aprox = 4096;              // 4 KB
    const int enteros_por_pagina = tam_pagina_aprox / sizeof(int);
    const int paginas = 8;

    vector<int> memoria(paginas * enteros_por_pagina, 0);

    cout << "=== Ejemplo de memoria virtual ===\n";
    cout << "Tamaño aproximado de pagina: " << tam_pagina_aprox << " bytes\n";
    cout << "Enteros por pagina: " << enteros_por_pagina << "\n";
    cout << "Paginas reservadas: " << paginas << "\n\n";

    cout << "Direccion virtual inicial del arreglo: "
              << static_cast<const void*>(&memoria[0]) << "\n\n";

    cout << "Accediendo al primer entero de cada pagina:\n";

    for (int pagina = 0; pagina < paginas; ++pagina) {
        int indice = pagina * enteros_por_pagina;
        memoria[indice] = pagina + 1;

        cout << "Pagina " << pagina
                  << " -> indice " << indice
                  << " -> direccion " << static_cast<const void*>(&memoria[indice])
                  << " -> valor " << memoria[indice] << "\n";
    }

    return 0;
}
