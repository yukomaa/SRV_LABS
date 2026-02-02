#include <iostream>
#include <thread>
#include <mutex>
#include <chrono>

int coins = 101;
int Bob_coins = 0;
int Tom_coins = 0;
std::mutex m;

void thief(const std::string& name, int& own_coins) {
    while (true) {
        m.lock();
        if (coins <= 0) {
            m.unlock();
            break;
        }
        --coins;
        ++own_coins;
        m.unlock();

        std::this_thread::sleep_for(std::chrono::nanoseconds(1)); 
    }
}

int main() {
    std::thread t1(thief, "BOB", std::ref(Bob_coins));
    std::thread t2(thief, "TOM", std::ref(Tom_coins));

    t1.join();
    t2.join();

    std::cout << "BOB: " << Bob_coins << "\n";
    std::cout << "TOM: " << Tom_coins << "\n";
    std::cout << "Total: " << Bob_coins + Tom_coins << "\n";

    return 0;
}