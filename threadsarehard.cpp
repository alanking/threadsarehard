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
    bool running;
    std::string my_string;
} thread_struct;
std::vector<thread_struct> structs;

bool threads_available{};
std::mutex threads_available_m;
std::condition_variable ta_cv;

void task(int i) {
    // Make parent aware of existence
    {
        std::unique_lock<std::mutex> l(m);
    }
    cv.notify_all();

    while(true) {
        // Acquire blackboard lock and wait for this thread's row to be set to running
        std::unique_lock<std::mutex> l(m);
        auto& ts = structs[i];
        //std::cout << std::this_thread::get_id() << " watching ts..." << &ts << std::endl;
        work_cv.wait(l, [&]{/*std::cout << std::this_thread::get_id() << "..." << ts.running << std::endl;*/ return ts.running;});

        // Actual "task"
        std::cout << &ts << ".my_string:" << ts.my_string << std::endl;
        std::this_thread::sleep_for(std::chrono::milliseconds(100));

        // Become available and notify
        ts = thread_struct{};
        {
            //std::cout << std::this_thread::get_id() << " available now" << std::endl;
            std::unique_lock<std::mutex> at_l(threads_available_m);
            threads_available = true;
        }
        ta_cv.notify_all();
    }
}

int main() {
    const int thread_count{5};
    std::vector<std::thread> threads;
    structs.reserve(thread_count); // HACK(?)
    for (int i{}; i < thread_count; i++) {
        structs.emplace_back(thread_struct{});
        std::thread t{task, i};
        threads.emplace_back(std::move(t));
        {
            std::unique_lock<std::mutex> l(m);
            cv.wait(l);
        }
        //std::cout << "Created " << threads[i].get_id() << " with " << &structs[i] << std::endl;
    }
    {
        std::unique_lock<std::mutex> l(threads_available_m);
        threads_available = true;
    }

    int big{};
    while (true) {
        std::vector<std::string> strings;
        for (int i{}; i < thread_count * 2; i++) {
            strings.emplace_back("big_" + std::to_string(big) + "_i_" + std::to_string(i));
        }

        for (auto& s: strings) {
            bool sent{};
            while (!sent) {
                {
                    std::unique_lock<std::mutex> ta_l(threads_available_m);
                    if (!threads_available) {
                        // Wait for a thread to become available
                        ta_cv.wait(ta_l, []{return threads_available;});
                    }
                }
                std::unique_lock<std::mutex> l(m);
                for (auto& ts: structs) {
                    if (!ts.running) {
                        //std::cout << "setting my_string to " << s << " for ts " << &ts << std::endl;
                        ts.my_string = s;
                        ts.running = true;
                        sent = true;
                        break;
                    }
                }
                if (!sent) {
                    // Make sure nobody is waiting on a signal
                    //std::cout << "notify workers and wait for thread availablity" << std::endl;
                    //work_cv.notify_all();
                    work_cv.notify_all();
                    {
                        //std::cout << s << " not sent, no threads available" << std::endl;
                        std::unique_lock<std::mutex> ta_l(threads_available_m);
                        threads_available = false;
                    }
                    l.unlock();
                    std::this_thread::sleep_for(std::chrono::milliseconds(100));
                }
            }
        }

        work_cv.notify_all();
        big++;
        std::this_thread::sleep_for(std::chrono::seconds(2));
        std::cout << "------------" << std::endl;
    }

    //std::cout << "notifying..." << std::endl;
    work_cv.notify_all();
    for (auto& t: threads) {
        t.join();
    }
}
