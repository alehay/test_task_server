#include <cstdlib>
#include <iostream>
#include "../server/client_list.hpp"


int main (int argc, char * argv []) {


    uint8_t start = std::atoi(argv[1]);
    uint8_t step = std::atoi(argv[2]);

    sequence<uint8_t> test_seq(start, step);
    
    for (int i = 0; i < 10 ; ++i) {
        std::cout << +test_seq.get()  << " ";
        ++test_seq;
    }
    return 0;
}