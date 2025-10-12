#include <iostream>
#include <list>
#include <vector>
#include <bits/time.h>
#include <time.h>
#include <cstdlib>

#include "PmergeMe.hpp"

int main(int argc, char **argv) {

    // const char *_argv[] = {"PmergeMe", "18", "1", "5", "7", "8", "15", "12", "10", "21", "4", "20", "11", "9", "2", "6", "17", "16", "13", "3", "14", "19", NULL};
    // argv = const_cast<char **>(_argv);
    // argc = sizeof(_argv) / sizeof(_argv[0]) - 1;

    std::vector<int> ints_vector(static_cast<std::size_t>(argc - 1));
    for (int i = 1; i < argc; ++i) {
        ints_vector[static_cast<std::size_t>(i - 1)] = std::atoi(argv[i]);
    }

    std::list<int> ints_list;
    for (int i = 1; i < argc; ++i) {
        ints_list.push_back(std::atoi(argv[i]));
    }

    std::cout << "Before (std::vector): ";
    print_vector(ints_vector, 0, false);
    std::cout << "Before (std::list):   ";
    print_list(ints_list, 0, false);

    struct timespec start_vector, end_vector;
    clock_gettime(CLOCK_PROCESS_CPUTIME_ID, &start_vector);
    ford_johnson_vector(ints_vector);
    clock_gettime(CLOCK_PROCESS_CPUTIME_ID, &end_vector);

    struct timespec start_list, end_list;
    clock_gettime(CLOCK_PROCESS_CPUTIME_ID, &start_list);
    ford_johnson_list(ints_list);
    clock_gettime(CLOCK_PROCESS_CPUTIME_ID, &end_list);

    std::cout << "After (std::vector):  ";
    print_vector(ints_vector, 0, false);
    std::cout << "After (std::list):    ";
    print_list(ints_list, 0, false);

    std::cout << "Time to process a range of " << argc - 1 << " integers with std::vector: " << (end_vector.tv_sec - start_vector.tv_sec) * 1000000000 + end_vector.tv_nsec - start_vector.tv_nsec
              << " ns == " << (end_vector.tv_sec - start_vector.tv_sec) * 1000000 + (end_vector.tv_nsec - start_vector.tv_nsec) / 1000
              << " us == " << (end_vector.tv_sec - start_vector.tv_sec) * 1000 + (end_vector.tv_nsec - start_vector.tv_nsec) / 1000000
              << " ms == " << end_vector.tv_sec - start_vector.tv_sec + (end_vector.tv_nsec - start_vector.tv_nsec) / 1000000000 << " s" << std::endl;
    std::cout << "Number of comparisons (std::vector): " << number_of_comparisons_vector << std::endl;

    std::cout << "Time to process a range of " << argc - 1 << " integers with std::list:   " << (end_list.tv_sec - start_list.tv_sec) * 1000000000 + end_list.tv_nsec - start_list.tv_nsec
              << " ns == " << (end_list.tv_sec - start_list.tv_sec) * 1000000 + (end_list.tv_nsec - start_list.tv_nsec) / 1000
              << " us == " << (end_list.tv_sec - start_list.tv_sec) * 1000 + (end_list.tv_nsec - start_list.tv_nsec) / 1000000
              << " ms == " << end_list.tv_sec - start_list.tv_sec + (end_list.tv_nsec - start_list.tv_nsec) / 1000000000 << " s" << std::endl;
    std::cout << "Number of comparisons (std::list):   " << number_of_comparisons_list << std::endl;

    return 0;
}
