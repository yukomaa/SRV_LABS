#include <iostream>
#include <thread>
#include <atomic>
#include <chrono>
#include <string>

using namespace std;

class MyMutex {
private:
    std::atomic<int> state{ 0 }; 

public:
    void lock() {
        int expected = 0;
        while (!state.compare_exchange_weak(expected, 1, std::memory_order_acquire)) {
            expected = 0;
            std::this_thread::yield();
        }
    }

    void unlock() {
        state.store(0, std::memory_order_release);
    }
};

MyMutex m;  

int dish1 = 3000;
int dish2 = 3000;
int dish3 = 3000;

int eaten1 = 0;
int eaten2 = 0;
int eaten3 = 0;

atomic<bool> can_eat = false;
atomic<bool> can_serve = false;

atomic<bool> cook_quit = false;
atomic<bool> cook_not_paid = false;
atomic<bool> cook_fired = false;

void cook(int& efficiency) {
    auto start = chrono::steady_clock::now();
    while (true) {
        while (!can_serve && !cook_fired && !cook_quit) {
            std::this_thread::yield();
        }

        m.lock();
        if (chrono::steady_clock::now() - start >= chrono::seconds(5)) {
            cook_quit = true;
            m.unlock();
            break;
        }

        if (cook_fired) { m.unlock(); break; }

        if (eaten1 < 10000) {
            dish1 += efficiency;
        }
        if (eaten2 < 10000) {
            dish2 += efficiency;
        }
        if (eaten3 < 10000) {
            dish3 += efficiency;
        }

        if (eaten1 >= 10000 && eaten2 >= 10000 && eaten3 >= 10000) {
            cook_not_paid = true;
            m.unlock();
            break;
        }

        can_eat = true;
        can_serve = false;
        m.unlock();
        std::this_thread::sleep_for(std::chrono::microseconds(4));
    }
}

void eat(int& dish, int& eaten, int& gluttony) {
    while (true) {
        while (!can_eat && !cook_quit && !cook_fired) {
            std::this_thread::yield();
        }

        m.lock();
        if (cook_fired) { m.unlock(); break; }
        if (cook_quit) { m.unlock(); break; }

        if (dish >= gluttony) {
            dish -= gluttony;
            eaten += gluttony;
            if (eaten >= 10000) {
                can_eat = false;
                can_serve = true;
                m.unlock();
                break;
            }
        }
        else {
            cook_fired = true;
            m.unlock();
            break;
        }

        can_eat = false;
        can_serve = true;
        m.unlock();
        std::this_thread::sleep_for(std::chrono::microseconds(4));
    }
}

void situation(int gluttony, int efficiency) {
    dish1 = 3000;
    dish2 = 3000;
    dish3 = 3000;

    eaten1 = 0;
    eaten2 = 0;
    eaten3 = 0;

    can_eat = false;
    can_serve = true;

    cook_quit = false;
    cook_not_paid = false;
    cook_fired = false;

    std::thread cook_thread(cook, ref(efficiency));
    std::thread thread1(eat, ref(dish1), ref(eaten1), ref(gluttony));
    std::thread thread2(eat, ref(dish2), ref(eaten2), ref(gluttony));
    std::thread thread3(eat, ref(dish3), ref(eaten3), ref(gluttony));

    cook_thread.join();
    thread1.join();
    thread2.join();
    thread3.join();

    cout << "Результаты\n" << endl;
    if (cook_not_paid) {
        cout << "Куку не заплатили" << endl;
    }
    else if (cook_quit) {
        cout << "Кук уволился" << endl;
    }
    else if (cook_fired) {
        cout << "Кук уволен" << endl;
    }

    cout << "Толстяки съели: " << eaten1 << " " << eaten2 << " " << eaten3 << "." << endl;
    cout << "Вся еда: " << dish1 << " " << dish2 << " " << dish3 << "." << endl;
}

int main()
{
    setlocale(LC_ALL, "rus");
    cout << "Gluttony - 1, efficiency - 1" << endl;
    situation(1, 1);
    cout << "\n" << endl;
    cout << "Gluttony - 4000, efficiency - 10" << endl;
    situation(4000, 10);
    cout << "\n" << endl;
    cout << "Gluttony - 100, efficiency - 100" << endl;
    situation(100, 100);
}