#include <iostream>
#include <future>
#include <thread>

using namespace std;

int main(){

    // crear promesa (el hijo promete 'enviar' una se;al)
    promise<void> p;

    // obtenemos el futuro asociado (el hilo padre esperara este futuro)
    future<void> f = p.get_future();

    // Pasamos la promesa al lambda por referencia para poder modificarla

    auto child = [&p]() -> void{
        cout << "child" << endl;

        // Cumplio la promesa. esta se;al es 'one shot'
        p.set_value();
    };

    thread t(child);
    t.detach(); // separamos el hilo


    // Custom join method: El padre se detiene aca hasta que el hijo llame a set_value
    f.wait();

    cout << "parent" << endl;

    return 0;
}
