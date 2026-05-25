#include <cmath>
#include <cstdint>
#include <functional>
#include <iostream>
#include <thread>
#include <chrono>
#include <vector>

using namespace std;

/*
 * - Existen dependencias de datos (data dependecies)?
 * No hay dependencias de datos entre las iteraciones del bucle (loop-carried dependencies).
 * Cada llamada a la funcion Riemann_Zeta(2,k) depende unicamente del valor actual de k. El calculo
 * de una iteracion no requiere el resultado de una iteracion anterior ni posterior. Esto significa
 * que el bucle es un candidato perfecto para el paralelismo, un patron conocido como Embarrasingly Parallel.

- Hay variables compartidas?
Si. El vector X es una variable compartida entre todos los hilos. Tambien lo son las variables de solo lectura n
y el parametro de entrada. Sin embargo, el hecho de que X sea compartida no genera un problema (condicion de carrera o data race).
Esto se debe a que cada hilo escribira en un indice k estrictamente diferente y unico (X[k] = ...) Como las direcciones de memoria
modificadas son independientes, no necesitamos mecanismos de sincronizacion costosos como semaforos o mutex para proteger la escritura.
*/

double Riemann_zeta(double s, uint64_t k){

    double result = 0.0;

    for(uint64_t i=1; i<k; i++){
        for(uint64_t j=1; j<k; j++){
            result += (2*(i&1)-1)/pow(i+j,s);
        }
    }
    return result*pow(2,s);
}


void worker(vector<double>& X, uint64_t start, uint64_t end){

    for(uint64_t k = start; k < end; k++){
        X[k] = Riemann_zeta(2, k);
    }
}

void calculate_riemman_zeta_parallel(vector<double>& X, uint64_t n){


    // Determinar cuantos hilos concurrentes soporta el hardware
    //unsigned int num_threads = thread::hardware_concurrency();
    //if(num_threads == 0) num_threads = 4;
    unsigned int num_threads = 8;

    vector<thread> threads;

    // Funcion lambda que ejecutara cada hilo
    // Recibe un rango [start, end) poara iterar
    //auto worker = [&](uint64_t start, uint64_t end){
    //    for(uint64_t k = start; k < end; k++){
    //        X[k] = Riemann_zeta(2, k);
    //    }
    //};

    // Calcular el tama;o del bloque para cada hilo
    uint64_t chunk_size = n / num_threads;
    uint64_t remainder = n % num_threads;
    uint64_t current_start = 0;

    auto start_time = chrono::high_resolution_clock::now();

    // Lanzar los hilos
    for(unsigned int i = 0; i < num_threads; i++){
        // Distribuimos el residuo de la divison entre los primeros hilos
        uint64_t current_end = current_start + chunk_size + (i < remainder ? 1 : 0);

        // Creamos y ejecutamos el hilo pasando el rango correspondiente
        threads.push_back(thread(worker, ref(X), current_start, current_end));
        current_start = current_end;
    }

    // Barrera de sincronizacion el hilo principal espera que todos terminen
    for(auto &t : threads){
        if(t.joinable()){
            t.join();
        }
    }

    auto end_time = chrono::high_resolution_clock::now();
    auto duration = chrono::duration_cast<chrono::milliseconds>(end_time - start_time);

    cout << "Tiempo de ejecucion Bloques: " << duration.count() << " milisegundos" << endl;
    cout << "Threads: " << num_threads << endl;
    cout << "Size n: " << n << endl;

}

void workerCyclic(vector<double> &X, uint64_t n, int thread_id, int num_threads){

    for(uint64_t k = thread_id; k<n; k+=num_threads) {
        X[k] = Riemann_zeta(2, k);
    }

}

void workerBlockCyclic(vector<double> &X, uint64_t n, int thread_id, int num_threads, uint64_t block_size){

    for(uint64_t block_start = thread_id * block_size; block_start < n; block_start += num_threads * block_size){

        uint64_t block_end = min(block_start + block_size, n); //Evita salirnos del arreglo
        for(uint64_t k = block_start; k < block_end; k++) {
            X[k] = Riemann_zeta(2, k);
        }
    }

}

int main(){

    uint64_t n = 3500;
    vector<double> X(n);
    int num_threads = 8;
    vector<thread> threads;
    vector<thread> threads2;
    uint64_t block_size = 50;

    auto start_time = chrono::high_resolution_clock::now();

    for(unsigned int i = 0; i< num_threads; i++){
        threads.push_back(thread(workerCyclic,ref(X), n, i, num_threads));
    }

    for(auto& t : threads){
        if(t.joinable()) t.join();
    }

    auto end_time = chrono::high_resolution_clock::now();

    auto duration = chrono::duration_cast<chrono::milliseconds>(end_time - start_time);

    cout << "Tiempo de ejecucion Ciclico: " << duration.count() << " milisegundos\n";
    cout << "Threads: " << num_threads << endl;
    cout << "Size n: " << n << endl;

    calculate_riemman_zeta_parallel(X,  n);

    auto start_time2 = chrono::high_resolution_clock::now();

    for(unsigned int i = 0; i < num_threads; i++){
        threads2.push_back(thread(workerBlockCyclic, ref(X), n, i, num_threads, block_size));
    }

    for(auto& tx : threads2){
        if(tx.joinable()) tx.join();
    }

    auto end_time2 = chrono::high_resolution_clock::now();

    auto duration2 = chrono::duration_cast<chrono::milliseconds>(end_time2 - start_time2);
    cout << "Tiempo de ejecucion Bloque Ciclico: " << duration2.count() << " milisegundos\n";
    cout << "Threads: " << num_threads << endl;
    cout << "Size n: " << n << endl;
    cout << "Block size: " << block_size << endl;

}
