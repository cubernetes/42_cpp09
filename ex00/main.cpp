#include <iostream>
#include <stdlib.h>

#include "BitcoinExchange.hpp"

int main(int argc, char **argv) {
    if (argc != 2) {
        std::cout << "Usage: " << argv[0] << " <file>" << std::endl;
        return EXIT_FAILURE;
    }

    return BitcoinExchange(const_cast<char *>("data.csv"), argv[1]);
}
