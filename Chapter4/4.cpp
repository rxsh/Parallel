#include <condition_variable>
#include <iostream>
#include <mutex>
#include <thread>

using namespace std;

bool done = false;
mutex m;
condition_variable c;

// locking explicito (unique_lock_manual)
void custom_join_explicit(){
    unique_lock<mutex> lock(m);
    c.wait(lock,[] { return done; });
}

auto child_explicit = []() -> void {
    cout << "[explicit] child\n";

    // Punto b: setear done = true dentro de un scope bloqueado
    {
        unique_lock<mutex> lock(m);
        done = true;
    }
    c.notify_one();
};

// scoped lock (lock_guard)
bool done2 = false;
mutex m2;
condition_variable c2;

void custom_join_scoped(){
    unique_lock<mutex> lock(m2);
    c2.wait(lock, [] {return done2;});
}

auto child_scoped = []() -> void {
    cout << "[scoped]   child.\n";

    {
        lock_guard<mutex> guard(m2);
        done2 = true;
    }
    c2.notify_one();
};

// notify dentro del lock

bool done3 = false;
mutex m3;
condition_variable c3;

void custom_join_notify_inside(){
    unique_lock<mutex> lock(m3);
    c3.wait(lock, [] { return done3; });
}

auto child_notify_inside = []() -> void {
    cout << "[notify-in] child\n";

    unique_lock<mutex> lock(m3);
    done3 = true;
    c3.notify_one();
};


int main(){

    // locking explicito
    {
        done = false;
        thread t(child_explicit);
        t.detach();
        custom_join_explicit();          // bloquea hasta que el hijo termina
        std::cout << "[explicit] parent\n\n";
    }

    // scoped lock en el hijo
    {
        done2 = false;
        thread t(child_scoped);
        t.detach();
        custom_join_scoped();
        std::cout << "[scoped]   parent\n\n";
    }

    //  notify dentro del lock
    {
        done3 = false;
        thread t(child_notify_inside);
        t.detach();
        custom_join_notify_inside();
        cout << "[notify-in] parent\n";
    }

    return 0;
}
