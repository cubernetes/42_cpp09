#include <cstddef>
#include <cstring>
#include <iostream>
#include <limits>

std::size_t number_of_comparisons;
int *tmp_chunk;

void print_array(int ints[], std::size_t size, std::size_t chunk_size) {
    for (std::size_t idx = 0; idx < size; ++idx) {
        bool is_main_chain = ((idx + 1) & (chunk_size * 2 - 1)) == 0;
        if (idx > 0)
            std::cout << ", ";
        if (is_main_chain)
            std::cout << "\033[101;30m";
        std::cout << ints[idx];
        if (is_main_chain)
            std::cout << "\033[m";
    }
    std::cout << std::endl;
}

// assumes bound check has already been made, might segfault otherwise
int get_biggest_in_chunk(int ints[], std::size_t idx, std::size_t chunk_size) { return ints[idx + chunk_size - 1]; }

// ascending
// assumes bound check has already been made, might segfault otherwise
void sort_two_chunks(int ints[], std::size_t size, std::size_t idx, std::size_t chunk_size) {
    // ignore straggler chunk
    if (idx + chunk_size * 2 - 1 >= size)
        return;

    ++number_of_comparisons;

    // biggest element is always farthest to the right
    std::size_t biggest_element_in_first_chunk = get_biggest_in_chunk(ints, idx, chunk_size);
    std::size_t biggest_element_in_second_chunk = get_biggest_in_chunk(ints, idx + chunk_size, chunk_size);

    // already sorted
    if (biggest_element_in_first_chunk < biggest_element_in_second_chunk)
        return;

    std::memmove(tmp_chunk, ints + idx, chunk_size * sizeof(*ints));
    std::memmove(ints + idx, ints + idx + chunk_size, chunk_size * sizeof(*ints));
    std::memmove(ints + idx + chunk_size, tmp_chunk, chunk_size * sizeof(*ints));
}

// returning `false` is the base case: we only have 1 or 0 elements, which are already, necessarily sorted
bool establish_main_chain(int ints[], std::size_t size, std::size_t chunk_size) {
    if (chunk_size * 2 > size)
        return false;
    for (std::size_t idx = 0; idx < size; idx += chunk_size * 2)
        sort_two_chunks(ints, size, idx, chunk_size);
    return true;
}

void binary_insert(int ints[], std::size_t size, std::size_t idx, std::size_t chunk_size) { int number_to_insert = get_biggest_in_chunk(ints, idx, chunk_size); }
// problem: how do we do binary search/insertion when part of the array is half as dense as the other half?

void binary_insertion_downwards(int ints[], std::size_t size, std::size_t chunk_size, std::size_t jacobsthal, std::size_t prev_jacobsthal) {
    --jacobsthal; // decrementing since `ints` is 0-indexed
    --prev_jacobsthal;

    if (jacobsthal >= size)
        jacobsthal = size - 1;

    for (std::size_t jacobs_idx = jacobsthal; jacobs_idx > prev_jacobsthal; --jacobs_idx) {
        std::size_t actual_idx = jacobs_idx * chunk_size * 2;
        binary_insert(ints, size, actual_idx, chunk_size);
    }
}

void binary_insertion_accordion_to_jacobsthal_numbering(int ints[], std::size_t size, std::size_t chunk_size) {
    std::size_t prev_jacobsthal = 1;
    std::size_t jacobsthal = 3;
    while (true) {
        binary_insertion_downwards(ints, size, chunk_size, jacobsthal, prev_jacobsthal);
        jacobsthal += prev_jacobsthal * 2;
        prev_jacobsthal = jacobsthal - prev_jacobsthal * 2;
    }
}

void sort_main_chain(int ints[], std::size_t size, std::size_t chunk_size) {
    if (!establish_main_chain(ints, size, chunk_size))
        return;
    std::cout << "This is the main chain with chunk size " << chunk_size << std::endl;
    print_array(ints, size, chunk_size);
    sort_main_chain(ints, size, chunk_size << 1);
    binary_insertion_accordion_to_jacobsthal_numbering(ints, size, chunk_size);
}

void ford_johnson(int ints[], std::size_t size) {
    tmp_chunk = new int[size]();
    sort_main_chain(ints, size, 1);
    delete[] tmp_chunk;
}

int main(void) {
    int ints[] = {18, 1, 5, 7, 8, 15, 12, 10, 21, 4, 20, 11, 9, 2, 6, 17, 16, 13, 3, 14, 19};
    std::cout << "INITIAL ARRAY" << std::endl;
    print_array(ints, sizeof(ints) / sizeof(*ints), std::numeric_limits<std::size_t>::max());
    ford_johnson(ints, sizeof(ints) / sizeof(*ints));
    std::cout << "FINAL ARRAY" << std::endl;
    print_array(ints, sizeof(ints) / sizeof(*ints), std::numeric_limits<std::size_t>::max());
    return 0;
}
