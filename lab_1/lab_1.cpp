#include <iostream>
#include <thread>
#include <mutex>
#include <string>
#include <chrono>

std::mutex mtx;

long long factorial(int n) {
    long long res = 1;
    for (int i = 2; i <= n; ++i)
        res *= i;
    return res;
}

void Func(std::string name) {

    const int N = 5'000'000; 
    for (int i = 0; i < N; ++i) {
        volatile long long r = factorial(10);
    }
    std::lock_guard<std::mutex> lock(mtx);
}

int main() {
    setlocale(LC_ALL, "rus");
    using Clock = std::chrono::high_resolution_clock;
    using ms = std::chrono::milliseconds;

    //последовательно
    auto start_seq = Clock::now();
    Func("Seq1");
    Func("Seq2");
    auto end_seq = Clock::now();
    auto seq_time = std::chrono::duration_cast<ms>(end_seq - start_seq).count();

    std::cout << "Последовательно: " << seq_time << " ms\n";

    //параллельно
    auto start_par = Clock::now();
    std::thread t1(Func, "Thread1");
    std::thread t2(Func, "Thread2");

    t1.join();
    t2.join();
    auto end_par = Clock::now();
    auto par_time = std::chrono::duration_cast<ms>(end_par - start_par).count();

    std::cout << "параллельно: " << par_time << " ms\n";

    std::cout << "Сравнение: " << static_cast<double>(seq_time) / par_time << " раз\n";

    return 0;
}