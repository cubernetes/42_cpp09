#pragma once

#include <list>
#include <vector>

void ford_johnson_list(std::list<int> &ints);
void ford_johnson_vector(std::vector<int> &ints);

void print_vector(std::vector<int> &ints, std::size_t chunk_size, bool color_chunks);
void print_list(std::list<int> &ints, std::size_t chunk_size, bool color);

extern std::size_t number_of_comparisons_list;
extern std::size_t number_of_comparisons_vector;
