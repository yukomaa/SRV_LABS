#include <iostream>
#include <thread>
#include <mutex>
#include <string>
#include <chrono>
#include <cstdint>

//std::mutex m;

void Func(std::string name) {
    auto start = std::chrono::steady_clock::now();
    long double i = 0;

    while (std::chrono::duration<double>(std::chrono::steady_clock::now() - start).count() < 1.0) {
        ++i;
    }

    //m.lock();
    std::cout << name << ": " << i << std::endl;
    //m.unlock();
}

int main() {
    std::thread thread1(Func, "t1");
    std::thread thread2(Func, "t2");
    std::thread thread3(Func, "t3");

    thread1.join();
    thread2.join();
    thread3.join();

    system("pause");
    return 0;
}