#include <condition_variable>
#include <chrono>
#include <mutex>
#include <iostream>
#include <string>
#include <thread>
#include <vector>

std::mutex m;
std::condition_variable cv;
std::condition_variable work_cv;

typedef struct {
    bool go;
    std::string my_string;
} thread_struct;

void task(thread_struct& ts) {
    {
        std::unique_lock<std::mutex> l(m);
    }
    cv.notify_all();
    std::unique_lock<std::mutex> l(m);
    std::cout << std::this_thread::get_id() << " watching ts..." << &ts << std::endl;
    work_cv.wait(l, [&]{return ts.go;});
    std::cout << &ts << ".my_string:" << ts.my_string << std::endl;
    ts = thread_struct{};
}

int main() {
    const int thread_count{5};
    std::vector<std::thread> threads;
    std::vector<thread_struct> structs;
    std::vector<std::string> strings;
    structs.reserve(thread_count); // HACK(?)
    for (int i{}; i < thread_count; i++) {
        structs.emplace_back(thread_struct{});
        std::thread t{task, std::ref(structs[i])};
        threads.emplace_back(std::move(t));
        strings.emplace_back("i" + std::to_string(i));
        {
            std::unique_lock<std::mutex> l(m);
            cv.wait(l);
        }
        std::cout << "Created " << threads[i].get_id() << " with " << &structs[i] << std::endl;
    }

    for (auto& s: strings) {
        std::unique_lock<std::mutex> l(m);
        for (auto& ts: structs) {
            if (!ts.go) {
                std::cout << "setting my_string to " << s << " for ts " << &ts << std::endl;
                ts.my_string = s;
                ts.go = true;
                break;
            }
            else {
                std::cout << "go is true for ts " << &ts << std::endl;
            }
        }
    }

    std::cout << "notifying..." << std::endl;
    work_cv.notify_all();
    for (auto& t: threads) {
        t.join();
    }
}
