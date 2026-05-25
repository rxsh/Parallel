#include <condition_variable>
#include <iostream>
#include <mutex>
#include <thread>

using namespace std;

bool done = false;
mutex m;
condition_variable c;

void custom_join(){
    unique_lock<mutex> lock(m);
    c.wait(lock,[] { return done; });
}

int main(){

    /*
    auto child = []() -> void {
        cout << "child" << endl;

        // Modificamos 'done' dentro de scope bloqueado
        {

            lock_guard<mutex> lock(m);
            done = true;
        }

        c.notify_one();
    };
     */

    auto child = []() -> void {
        cout << "child" << endl;

        // Bloque explicito
        {
            m.lock();
            done = true;
            m.unlock();
        }

        c.notify_one();
    };



    thread t(child);
    t.detach(); // Lo separamos porque vamos a gestionar la espera nosotros mismos

    // Llamamos a nuestra sincronizacion manual
    custom_join();

    cout << "parent" << endl;

    return 0;
}
