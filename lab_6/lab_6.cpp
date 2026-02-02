#include <iostream>
#include <coroutine>
#include <chrono>
#include <thread>
#include <iomanip>

const std::string myname = "EgorMikhailov";

struct Task {
    struct promise_type {
        int current_value = 0;

        Task get_return_object() {
            return Task{ std::coroutine_handle<promise_type>::from_promise(*this) };
        }

        std::suspend_never initial_suspend() noexcept { return {}; }
        std::suspend_always final_suspend() noexcept { return {}; }

        void return_void() noexcept {}  

        void unhandled_exception() { std::terminate(); }

        std::suspend_always yield_value(int value) noexcept {
            current_value = value;
            return {};
        }
    };

    std::coroutine_handle<promise_type> handle;

    explicit Task(std::coroutine_handle<promise_type> h) : handle(h) {}
    ~Task() { if (handle) handle.destroy(); }

    void resume() { handle.resume(); }
    bool done() const { return handle.done(); }
    int value() const { return handle.promise().current_value; }
};

Task long_computation(int steps) {
    for (int i = 1; i <= steps; ++i) {
        co_yield i;
        std::this_thread::sleep_for(std::chrono::milliseconds(50));
    }
    co_return;
}

void print_progress(int current, int total) {
    const size_t bar_width = myname.length() + 1;
    float percent = static_cast<float>(current) / total * 100.0f;
    size_t pos = static_cast<size_t>(percent / 100.0f * bar_width);

    std::string bar(bar_width, ' ');
    for (size_t i = 0; i < pos; ++i) {
        bar[i] = myname[i];
    }
    if (pos > 0 && pos <= bar_width) {
        bar[pos - 1] = '>';
    }

    std::ostringstream oss;
    oss << std::fixed << std::setprecision(1) << percent << "%";
    std::string pct_str = oss.str();

    std::string line = "[" + bar + "] " + pct_str;

    if (line.length() < 40)
        line.append(40 - line.length(), ' ');

    std::cout << "\r" << line;
    std::cout.flush();
}

int main() {
    constexpr int TOTAL_STEPS = 100;
    auto coro = long_computation(TOTAL_STEPS);

    while (!coro.done()) {
        coro.resume();
        print_progress(coro.value(), TOTAL_STEPS);
        std::this_thread::yield();
    }
    std::cout << std::endl;
    return 0;
}