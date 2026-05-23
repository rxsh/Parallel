#include <cmath>
#include <cstdint>
#include <iostream>
#include <thread>
#include <chrono>
#include <vector>

using namespace std;

double Riemann_zeta(double s, uint64_t k){

    double result = 0.0;

    for(uint64_t i=1; i<k; i++){
        for(uint64_t j=1; j<k; j++){
            result += (2*(i&1)-1)/pow(i+j,s);
        }
    }
    return result*pow(2,s);
}


// cada hilo recibe un bloque contiguo
void parallel_block(vector<double>& X, unsigned num_threads){
    size_t n = X.size();

    vector<thread> threads;
    size_t chunk = (n + num_threads - 1) / num_threads;

    for(unsigned t=0; t < num_threads; t++){
        size_t lo = t * chunk;
        size_t hi = min(lo + chunk, n);
        threads.emplace_back([&X,lo,hi](){
            for(size_t k = lo; k < hi; k++){
                X[k] = Riemann_zeta(2, k);
            }
        });
    }
    for(auto& th : threads) th.join();

}

// Particion ciclica Round Robin
void parallel_cyclic(vector<double>&X, unsigned num_threads){

    size_t n = X.size();
    vector<thread> threads;

    for(unsigned t = 0; t < num_threads; t++){
        threads.emplace_back([&X,n,t,num_threads]() {
            for(size_t k = t; k<n; k+= num_threads){
                X[k] = Riemann_zeta(2, k);
            }
        });
    }
    for(auto& th : threads) th.join();
}

// Combinada localidad cache (bloques) con balance(ciclico)

void parallel_block_cyclic(vector<double>&X, unsigned num_threads, size_t block_size=4){

    size_t n = X.size();
    vector<thread> threads;
    size_t num_blocks = (n + block_size -1)/block_size;

    for(unsigned t = 0; t<num_threads; t++){
        threads.emplace_back([&X,n,t,num_threads, block_size, num_blocks](){
            for(size_t b = t; b<num_blocks; b+=num_threads){
                size_t lo = b * block_size;
                size_t hi = min(lo + block_size, n);
                for(size_t k = lo; k < hi; k++){
                    X[k] = Riemann_zeta(2, k);
                }
            }
        });
    }
    for(auto& th : threads) th.join();

}


int main(){

    // Faltar relacionar las pruebas

}
