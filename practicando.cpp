// Metodologia Foster


// Implementacion PThreads
thread_count;
double sum = 0.0;
pthread_create_mutex mutex;

void *Operation(void* rank){

    int my_rank = (int)rank;
    int my_n = my_rank / thread_count;
    int inicio = my_rank * my_n;
    int fin = inicio + my_n;
    double factor = 1.0;

    if(my_rank % 2 == 0){
        factor = 1.0;
    } else{
        factor = -1.0;
    }

    for(int i=0; i<= fin; i++, factor = -factor){
        double my_sum = 0.0;
        my_sum += factor/(2*i+1);
    }

    mutex lock();
    sum += my_sum;
    mutex unlock();

}

// Implementacion OpenMP

double factor = 1.0, sum = 0.0;
#pragma omp parallel for num_threads(thread_count) default(none) private(factor) shared(sum)
for(int i=0; i<n; i++){
    factor *= -1;
    #pragma omp critical
    sum += factor/(2*i+1);
}
pi = 4.0*sum;
