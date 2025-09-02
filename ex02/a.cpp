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

// does a normal binary insert on an array, however, after (and including) a threshold value, binary search should
// only consider every other element.
bool virtual_binary_insert(int ints[], std::size_t size, std::size_t idx, std::size_t chunk_size, std::size_t binary_insertion_threshold) {
    if (idx + chunk_size - 1 >= size) {
        // out of bounds, don't increase threshold
        return false;
    }
    int number_to_insert = get_biggest_in_chunk(ints, idx, chunk_size);
    std::cout << "Number to insert: " << number_to_insert << std::endl;
    return true;
}
// Further Notes:
//
// Problem: how do we do binary search/insertion when part of the array is half as dense as the other half?
//
// main chain:    c1, c2, a2, a3, a4, a5, a6, a7, a8, a9, a10
// pending chain:        b2, b3, b4, b5, b6, b7, b8, b9, b10, b11
// [c1, c2, b2, a2, b3, a3, b4, a4, b5, a5, b6, a6, b7, a7, b8, a8, b9, a9, b10, a10, b11]
//
// let's insert b3
// for that, we need to do binary search on [c1, c2, a2]. However, inside the array, we find
// [c1, c2, b2, a2]. Therefore, we need a function that maps the virtual binary search indices to
// the actual array indices. Something like
// - `actual_idx = virtual_idx < threshold ? virtual_idx : threshold + 1 + (virtual_idx - threshold) * 2
// In our example, threshold is 2 (assuming 0-indexing)
// Going through all 3 possible values of virtual_idx, we can calculate actual_idx:
// actual_idx = 0 < 2 ? 0 : 2 + 1 + (0 - 2) * 2 = 0 // normal idx
// actual_idx = 1 < 2 ? 1 : 2 + 1 + (1 - 2) * 2 = 1 // normal idx
// actual_idx = 2 < 2 ? 2 : 2 + 1 + (2 - 2) * 2 = 3 // threshold jump!
// If we had a 4th element, our formula would go in steps of 2, which is correct.
//
// Let's derive a general method for determining the threshold value.
//
// Example run through (threshold in quotes):
// [c1, c2, "b2", a2, b3, a3, b4, a4, b5, a5, b6, a6, b7, a7, b8, a8, b9, a9, b10, a10, b11] // after first recursive call
// [c1, c2, c3, "b2", a2, a3, b4, a4, b5, a5, b6, a6, b7, a7, b8, a8, b9, a9, b10, a10, b11]
// [c1, c2, c3, c4, c5, c6, "b4", a4, b5, a5, b6, a6, b7, a7, b8, a8, b9, a9, b10, a10, b11]
// [c1, c2, c3, c4, c5, c6, c7, "b4", a4, a5, b6, a6, b7, a7, b8, a8, b9, a9, b10, a10, b11]
// [c1, c2, c3, c4, c5, c6, c7, c8, c9, c10, "b6", a6, b7, a7, b8, a8, b9, a9, b10, a10, b11]
// [c1, c2, c3, c4, c5, c6, c7, c8, c9, c10, c11, "b6", a6, b7, a7, b8, a8, b9, a9, b10, a10]
// [c1, c2, c3, c4, c5, c6, c7, c8, c9, c10, c11, c12, "b6", a6, b7, a7, b8, a8, b9, a9, a10]
// [c1, c2, c3, c4, c5, c6, c7, c8, c9, c10, c11, c12, c13, "b6", a6, b7, a7, b8, a8, a9, a10]
// [c1, c2, c3, c4, c5, c6, c7, c8, c9, c10, c11, c12, c13, c14, "b6", a6, b7, a7, a8, a9, a10]
// [c1, c2, c3, c4, c5, c6, c7, c8, c9, c10, c11, c12, c13, c14, c15, "b6", a6, a7, a8, a9, a10]
// [c1, c2, c3, c4, c5, c6, c7, c8, c9, c10, c11, c12, c13, c14, c15, c16, c17, c18, c19, c20, c21] // sorted
//
// Example run through (threshold in quotes) with last jacobsthal index being one smaller:
// [c1, c2, "b2", a2, b3, a3, b4, a4, b5, a5, b6, a6, b7, a7, b8, a8, b9, a9, b10, a10] // after first recursive call
// [c1, c2, c3, "b2", a2, a3, b4, a4, b5, a5, b6, a6, b7, a7, b8, a8, b9, a9, b10, a10]
// [c1, c2, c3, c4, c5, c6, "b4", a4, b5, a5, b6, a6, b7, a7, b8, a8, b9, a9, b10, a10]
// [c1, c2, c3, c4, c5, c6, c7, "b4", a4, a5, b6, a6, b7, a7, b8, a8, b9, a9, b10, a10]
// [c1, c2, c3, c4, c5, c6, c7, c8, c9, c10, "b6", a6, b7, a7, b8, a8, b9, a9, b10, a10]
// [c1, c2, c3, c4, c5, c6, c7, c8, c9, c10, c11, "b6", a6, b7, a7, b8, a8, b9, a9, a10]
// [c1, c2, c3, c4, c5, c6, c7, c8, c9, c10, c11, c12, "b6", a6, b7, a7, b8, a8, a9, a10]
// [c1, c2, c3, c4, c5, c6, c7, c8, c9, c10, c11, c12, c13, "b6", a6, b7, a7, a8, a9, a10]
// [c1, c2, c3, c4, c5, c6, c7, c8, c9, c10, c11, c12, c13, c14, "b6", a6, a7, a8, a9, a10]
// [c1, c2, c3, c4, c5, c6, c7, c8, c9, c10, c11, c12, c13, c14, c15, c16, c17, c18, c19, c20] // sorted
// Evidently, the threshold values are the same, and the iteration for the b11 is skipped (and the threshold value is not incremented
// for that skipped iteration)
//
// both values 0-indexed
// values of idx_pending_chain | threshold
// 2                           | 2 (+1)
// 1                           | 3 (+1)
// 4                           | 6 (+3, new jacobsthal number)
// 3                           | 7 (+1)
// 10                          | 10 (+3, new jacobsthal number)
// 9                           | 11 (+1)
// 8                           | 12 (+1)
// 7                           | 13 (+1)
// 6                           | 14 (+1)
// 5                           | 15 (+1)
//
// Evidently, in our binary insertion loop, we can trivially update the threshold value every time:
// increase it by 1 every time, and by 2 every time a new jacobsthal number starts.

bool binary_insertion_downwards(int ints[], std::size_t size, std::size_t chunk_size, std::size_t jacobsthal, std::size_t prev_jacobsthal, std::size_t *binary_insertion_threshold) {
    --jacobsthal; // decrementing since `ints` is 0-indexed
    --prev_jacobsthal;

    if (jacobsthal >= size)
        jacobsthal = size - 1;

    bool did_something = false;
    for (std::size_t jacobs_idx = jacobsthal; jacobs_idx > prev_jacobsthal; --jacobs_idx) {
        std::size_t actual_idx = jacobs_idx * chunk_size * 2;
        if (virtual_binary_insert(ints, size, actual_idx, chunk_size, *binary_insertion_threshold)) {
            did_something = true;
            ++*binary_insertion_threshold;
        }
    }
    return did_something;
}

void binary_insertion_according_to_jacobsthal_numbering(int ints[], std::size_t size, std::size_t chunk_size) {
    std::size_t prev_jacobsthal = 1;
    std::size_t jacobsthal = 3;
    std::size_t binary_insertion_threshold = 2;

    while (true) {
        if (!binary_insertion_downwards(ints, size, chunk_size, jacobsthal, prev_jacobsthal, &binary_insertion_threshold))
            break;
        jacobsthal += prev_jacobsthal * 2;
        prev_jacobsthal = jacobsthal - prev_jacobsthal * 2;
        binary_insertion_threshold += 2;
    }
}

void sort_main_chain(int ints[], std::size_t size, std::size_t chunk_size) {
    if (!establish_main_chain(ints, size, chunk_size))
        return;
    std::cout << "This is the main chain with chunk size " << chunk_size << std::endl;
    print_array(ints, size, chunk_size);
    sort_main_chain(ints, size, chunk_size << 1);
    binary_insertion_according_to_jacobsthal_numbering(ints, size, chunk_size);
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
