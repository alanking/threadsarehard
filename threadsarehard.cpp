#include <condition_variable>
#include <chrono>
#include <mutex>
#include <iostream>
#include <string>
#include <thread>
#include <vector>

std::mutex m;
std::condition_variable cv;
int i{};

typedef struct {
};

void task() {
    {
        std::unique_lock<std::mutex> l(m);
        i++;
        std::cout << "i am " << std::this_thread::get_id() << " and i is " << i << std::endl;
    }
    cv.notify_all();
}

int main() {
    const int thread_count{5};
    std::vector<std::thread> threads;
    for (int j{}; j < thread_count; j++) {
        std::thread t{task};
        threads.emplace_back(std::move(t));
    }
    {
        std::unique_lock<std::mutex> l(m);
        cv.wait(l, []{return i >= thread_count;});
        i++;
        std::cout << "i is " << i << " and should be " << thread_count + 1 << std::endl;
    }
    for (auto& t: threads) {
        t.join();
    }
}
