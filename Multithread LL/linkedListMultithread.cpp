#include <iostream>
#include <pthread.h>
#include <vector>

using namespace std;

struct Nodo {
    int data;
    Nodo* next;

    explicit Nodo(int value) : data(value), next(nullptr) {}
};

class LinkedList {
private:
    Nodo* head;
    pthread_mutex_t mutex;

public:
    LinkedList() : head(nullptr) {
        pthread_mutex_init(&mutex, nullptr);
    }

    ~LinkedList() {
        clear();
        pthread_mutex_destroy(&mutex);
    }

    void insertar(int value) {
        pthread_mutex_lock(&mutex);

        Nodo* nuevo = new Nodo(value);
        nuevo->next = head;
        head = nuevo;

        pthread_mutex_unlock(&mutex);


    }

    bool search(int value) {
        pthread_mutex_lock(&mutex);

        Nodo* actual = head;
        while(actual != nullptr){
            if(actual->data == value){
                pthread_mutex_unlock(&mutex);
                return true;
            }
            actual = actual->next;
        }

        pthread_mutex_unlock(&mutex);
        return false;
    }

    bool delet(int value) {

        pthread_mutex_lock(&mutex);

        Nodo* actual = head;
        Nodo* anterior = nullptr;

        while (actual != nullptr) {
            if (actual->data == value) {
                if (anterior == nullptr) {
                    head = actual->next;
                } else {
                    anterior->next = actual->next;
                }
                delete actual;
                pthread_mutex_unlock(&mutex);
                return true;
            }
            anterior = actual;
            actual = actual->next;
        }

        pthread_mutex_unlock(&mutex);
        return false;
    }

    void print() {
        pthread_mutex_lock(&mutex);

        Nodo* actual = head;
        cout << "Lista: ";
        while (actual != nullptr) {
            cout << actual->data;
            if (actual->next != nullptr) {
                cout << " -> ";
            }
            actual = actual->next;
        }
        cout << endl;

        pthread_mutex_unlock(&mutex);
    }

private:
    void clear() {
        pthread_mutex_lock(&mutex);

        Nodo* actual = head;
        while (actual != nullptr) {
            Nodo* siguiente = actual->next;
            delete actual;
            actual = siguiente;
        }
        head = nullptr;

        pthread_mutex_unlock(&mutex);
    }
};

struct InsertArgs {
    LinkedList* list;
    vector<int> values;
};

struct SearchArgs {
    LinkedList* list;
    int value;
};

void* insertWorker(void* arg) {
    auto* data = static_cast<InsertArgs*>(arg);
    for (int value : data->values) {
        data->list->insertar(value);
    }
    return nullptr;
}

void* searchWorker(void* arg) {
    auto* data = static_cast<SearchArgs*>(arg);
    bool found = data->list->search(data->value);
    cout << "Buscar " << data->value << ": " << (found ? "encontrado" : "no encontrado") << endl;
    return nullptr;
}

int main() {
    LinkedList list;

    InsertArgs args1{&list, {1, 2, 3, 4, 5}};
    InsertArgs args2{&list, {6, 7, 8, 9, 10}};
    SearchArgs args3{&list, 7};

    pthread_t t1, t2, t3;

    pthread_create(&t1, nullptr, insertWorker, &args1);
    pthread_create(&t2, nullptr, insertWorker, &args2);
    pthread_join(t1, nullptr);
    pthread_join(t2, nullptr);

    pthread_create(&t3, nullptr, searchWorker, &args3);
    pthread_join(t3, nullptr);

    list.print();

    cout << "Eliminar 4: " << (list.delet(4) ? "ok" : "no encontrado") << endl;
    list.print();

    return 0;
}