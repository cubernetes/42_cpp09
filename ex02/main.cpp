#include "RPN.hpp"

#include <iostream>

/*
 * # Test division by 0 and division overflow
 *  ./RPN "1 0 / 0 1 - * 1 - 0 1 - *"
 */
int main(int ac, char **av) {
    if (ac != 2) {
        std::cout << "Usage: " << av[0]
                  << " RPN-EXPR\n\n    RPN-EXPR    expression in reverse-polish-notation,\n    e.g. \"1 2 3 - 4 + *\" results in \"3\"\n    verify with "
                     "https://paodayag.dev/reverse-polish-notation-js-parser"
                  << std::endl;
        return 1;
    }
    std::cout << rpnEval(av[1]) << std::endl;
    return 0;
}
