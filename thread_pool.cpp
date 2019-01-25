#include <chrono>
#include <ctime>
//#include <ctpl_stl.h>
#include <iostream>
#include <string>
#include <vector>

#include <boost/asio.hpp>

void thread_func(std::string& line) {
    std::cout << line << std::endl;
}

std::vector<std::string> create_list();
int main() {
    std::vector<std::string> print_list{create_list()};

    const int thread_count{1};
    boost::asio::thread_pool pool(thread_count);
    for (auto item: print_list) {
        boost::asio::post(pool, thread_func, item);
    }
    pool.join();
}








std::vector<std::string> create_list() {
    std::vector<std::string> print_list;
    print_list.push_back("Every single night");
    print_list.push_back("I endure the flight");
    print_list.push_back("Of little wings of white-flamed");
    print_list.push_back("Butterflies in my brain");
    print_list.push_back("These ideas of mine");
    print_list.push_back("Percolate the mind");
    print_list.push_back("Trickle down the spine");
    print_list.push_back("Swarm the belly, swelling to a blaze");
    print_list.push_back("That's where the pain comes in");
    print_list.push_back("Like a second skeleton");
    print_list.push_back("Trying to fit beneath the skin");
    print_list.push_back("I can't fit the feelings in");
    print_list.push_back("Every single night's alight with my brain");
    print_list.push_back("What'd I say to her?");
    print_list.push_back("Why'd I say to her?");
    print_list.push_back("What does she think of me?");
    print_list.push_back("That I'm not what I ought to be");
    print_list.push_back("That I'm what I try not to be");
    print_list.push_back("It's got to be somebody else's fault");
    print_list.push_back("I can't get caught");
    print_list.push_back("If what I am is what I am, 'cause I does what I does");
    print_list.push_back("Then brother, get back, 'cause my breast's gonna bust open");
    print_list.push_back("The rib is the shell and the heart is the yolk");
    print_list.push_back("I just made a meal for us both to choke on");
    print_list.push_back("Every single night's a fight with my brain");
    print_list.push_back("I just want to feel everything");
    print_list.push_back("So I'm gonna try to be still now");
    print_list.push_back("Gonna renounce the mill a little while and");
    print_list.push_back("If we had a double-king-sized bed");
    print_list.push_back("We could move in it and I'd soon forget");
    print_list.push_back("That what I am is what I am 'cause I does what I does");
    print_list.push_back("And maybe I'd relax, let my breast just bust open");
    print_list.push_back("My heart's made of parts of all that surround me");
    print_list.push_back("And that's why the devil just can't get around me");
    print_list.push_back("Every single night's alright, every single night's a fight");
    print_list.push_back("And every single fight's alright with my brain");
    print_list.push_back("I just want to feel everything");
    return print_list;
}
