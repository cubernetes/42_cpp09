#include <algorithm>
#include <bits/time.h>
#include <cstddef>
#include <cstdlib>
#include <cstring>
#include <ctime>
#include <iostream>
#include <time.h>
#include <vector>
#include <list>


/// vector start ///
std::size_t number_of_comparisons_vector;
std::vector<int> tmp_chunk;

void print_vector(std::vector<int> &ints, std::size_t chunk_size, bool color) {
    for (std::size_t idx = 0; idx < ints.size(); ++idx) {
        if (idx > 0)
            std::cout << ", ";

        if (!color) {
            std::cout << ints[idx];
            continue;
        }

        bool is_main_chain = ((idx + 1) & (chunk_size * 2 - 1)) == 0;
        bool is_end_of_chunk = ((idx + 1) & (chunk_size - 1)) == 0;

        if (is_main_chain)
            std::cout << "\033[101;30;1;4m";
        else if (is_end_of_chunk)
            std::cout << "\033[103;30;4m";
        else
            std::cout << "\033[103;30m";
        std::cout << ints[idx];
        if (is_main_chain || is_end_of_chunk)
            std::cout << "\033[m";
    }
    if (color)
        std::cout << "\033[m";
    std::cout << std::endl;
}

// assumes bound check has already been made, might segfault otherwise
int chunk_to_number_vector(std::vector<int> &ints, std::size_t idx, std::size_t chunk_size) { return ints[idx + chunk_size - 1]; }

// ascending
// assumes bound check has already been made, might segfault otherwise
void sort_chunk_pair_vector(std::vector<int> &ints, std::size_t size, std::size_t idx, std::size_t chunk_size) {
    // ignore straggler chunk
    if (idx + chunk_size * 2 - 1 >= size)
        return;

    // biggest element is always farthest to the right
    std::size_t biggest_element_in_first_chunk = chunk_to_number_vector(ints, idx, chunk_size);
    std::size_t biggest_element_in_second_chunk = chunk_to_number_vector(ints, idx + chunk_size, chunk_size);

    // std::cout << "Comparing biggest_element_in_first_chunk " << biggest_element_in_first_chunk << " against biggest_element_in_second_chunk " << biggest_element_in_second_chunk << std::endl;
    ++number_of_comparisons_vector;

    // already sorted
    if (biggest_element_in_first_chunk < biggest_element_in_second_chunk)
        return;

    std::memmove(tmp_chunk.data(), ints.data() + idx, chunk_size * sizeof(ints[0]));
    std::memmove(ints.data() + idx, ints.data() + idx + chunk_size, chunk_size * sizeof(ints[0]));
    std::memmove(ints.data() + idx + chunk_size, tmp_chunk.data(), chunk_size * sizeof(ints[0]));
}

// returning `false` is the base case: we only have 1 or 0 elements, which are already, necessarily sorted
bool sort_chunk_pairs_vector(std::vector<int> &ints, std::size_t size, std::size_t chunk_size) {
    if (chunk_size * 2 > size)
        return false; // need at least one chunk pair because the loop runs at least once
    for (std::size_t idx = 0; idx < size; idx += chunk_size * 2)
        sort_chunk_pair_vector(ints, size, idx, chunk_size);
    return true;
}

bool binary_insert_vector(std::vector<int> &ints, std::size_t size, std::size_t insert_chunk_idx, std::size_t chunk_size, std::size_t binary_search_end_chunk) {
    if (insert_chunk_idx + chunk_size - 1 >= size) {
        // out of bounds
        // std::cout << "Binary search would access index " << insert_chunk_idx + chunk_size - 1 << " (Jacobsthal logic) but we only have " << size << " numbers. Skipping" << std::endl;
        return false;
    }
    int number_to_insert = chunk_to_number_vector(ints, insert_chunk_idx, chunk_size);
    // std::cout << "Number to insert: " << number_to_insert << std::endl;

    // binary search
    std::size_t lo = 0;
    std::size_t hi = binary_search_end_chunk;
    std::size_t mi;
    int mi_num;

    while (lo < hi) {
        mi = (lo + hi) / chunk_size / 2 * chunk_size; // floor down to lower chunk
        mi_num = chunk_to_number_vector(ints, mi, chunk_size);
        // std::cout << "Comparing mi_num " << mi_num << " against number_to_insert " << number_to_insert << std::endl;
        ++number_of_comparisons_vector;
        if (mi_num <= number_to_insert)
            lo = mi + chunk_size; // exclude left array from search
        else if (mi >= chunk_size)
            hi = mi - chunk_size; // exclude right array from search
        else
            hi = 0;
    }
    int lo_num = chunk_to_number_vector(ints, lo, chunk_size);
    // std::cout << "Comparing lo_num " << lo_num << " against number_to_insert " << number_to_insert << std::endl;
    ++number_of_comparisons_vector;
    std::size_t insert_before_idx = lo_num < number_to_insert ? lo + chunk_size : lo;

    // insert
    std::size_t shift_chunks = (insert_chunk_idx - insert_before_idx) / chunk_size;
    std::memmove(tmp_chunk.data(), ints.data() + insert_before_idx, shift_chunks * chunk_size * sizeof(ints[0]));
    std::memmove(ints.data() + insert_before_idx, ints.data() + insert_chunk_idx, chunk_size * sizeof(ints[0]));
    std::memmove(ints.data() + insert_before_idx + chunk_size, tmp_chunk.data(), shift_chunks * chunk_size * sizeof(ints[0]));
    // std::cout << "After insertion" << std::endl;
    // print_vector(ints, chunk_size, true);

    return true;
}

bool binary_insertion_downwards_vector(std::vector<int> &ints, std::size_t size, std::size_t chunk_size, std::size_t jacobsthal, std::size_t prev_jacobsthal) {
    --jacobsthal; // decrementing since `ints` is 0-indexed
    --prev_jacobsthal;

    if (jacobsthal >= size)
        jacobsthal = size - 1;

    bool did_something = false;
    std::size_t chunk_idx = chunk_size * (size / chunk_size / 2 + jacobsthal);
    std::size_t binary_search_end_chunk = std::min(chunk_size * (size / chunk_size / 2 + prev_jacobsthal), chunk_size * (prev_jacobsthal + jacobsthal));

    for (std::size_t jacobs_idx = jacobsthal; jacobs_idx > prev_jacobsthal; --jacobs_idx) {
        if (binary_insert_vector(ints, size, chunk_idx, chunk_size, binary_search_end_chunk))
            did_something = true;
        else
            chunk_idx -= chunk_size;
    }
    return did_something;
}

// Mental note for myself:
// We need to prepare the array for binary insertion, because otherwise, the elements might be unevenly
// distributed, making binary search almost impossible. For example, the array might initially look like this:
// [c1, c2, b2, a2, b3, a3, b4, a4, b5, a5, b6, a6, b7, a7, b8, a8, b9, a9, b10, a10, b11]
// According the the Jacobsthal sequence, we want to insert b3 next. But we cannot do binary search, because
// there's a b2, making the interval of search not contiguous (requiring us to implement a pretty complicated
// 'virtual binary insert' function). Therefore, we could transform it to be this:
// [c1, c2, a2, b2, b3, a3, b4, a4, b5, a5, b6, a6, b7, a7, b8, a8, b9, a9, b10, a10, b11]
// Now we can do normal binary insertion for b3. If b3 lands after a2, then binary insertion for b2 would normally happen
// with c1 and c2. However, if b3 lands before a2, the b2's binary insertion will happen on c1, c2, and b3.
// More explicitly, after inserting b3, there are these two cases:
// [c1, c2, a2, b3, b2, a3, b4, a4, b5, a5, b6, a6, b7, a7, b8, a8, b9, a9, b10, a10, b11]
// [c1, c2, b3, a2, b2, a3, b4, a4, b5, a5, b6, a6, b7, a7, b8, a8, b9, a9, b10, a10, b11]
// Now, how do we keep track of that information when inserting b2?
// It actually doesn't matter! We can just always do the 'max' range, so c1, c2, and b3/a2, because the number of
// comparisons is fixed! Let's see a bigger example. We want to insert from b11 downwards:
// [c1, c2, c3, c4, c5, c6, c7, c8, c9, c10, b6, a6, b7, a7, b8, a8, b9, a9, b10, a10, b11]
// The array must be transformed to:
// [c1, c2, c3, c4, c5, c6, c7, c8, c9, c10, a6, a7, a8, a9, a10, b6, b7, b8, b9, b10, b11]
// Notice the b11 has to be binary inserted from c1 to a10 (15 elements, at least 4 comparisons)
// After that, the array might look like any of these:
// 1. [b11, c1, c2, c3, c4, c5, c6, c7, c8, c9, c10, a6, a7, a8, a9, a10, b6, b7, b8, b9, b10]
// 2. [c1, c2, c3, c4, c5, c6, c7, c8, c9, c10, b11, a6, a7, a8, a9, a10, b6, b7, b8, b9, b10]
// 3. [c1, c2, c3, c4, c5, c6, c7, c8, c9, c10, a6, b11, a7, a8, a9, a10, b6, b7, b8, b9, b10]
// 4. [c1, c2, c3, c4, c5, c6, c7, c8, c9, c10, a6, a7, a8, a9, b11, a10, b6, b7, b8, b9, b10]
// 5. [c1, c2, c3, c4, c5, c6, c7, c8, c9, c10, a6, a7, a8, a9, a10, b11, b6, b7, b8, b9, b10]
// Now notice that you can also insert b10 on 15 elements (-> 4 comparisons) in all cases:
// 1. b11, c1, ..., a9
// 2. c1, c2, ..., a9
// 3. c1, c2, ..., a9
// 4. c1, c2, ..., b11
// 5. c1, c2, ..., a10
// Notice that in the last case, including a10 is redundant (we already know that b10 < a10), but
// it doesn't hurt and it makes the implementation significantly easier.
// So let's pick possibility 3. and insert b10 at a random place:
// [c1, c2, c3, c4, c5, c6, c7, c8, b10, c9, c10, a6, b11, a7, a8, a9, a10, b6, b7, b8, b9]
// And now the range of 15 numbers for b9 is:
// c1, c2, ..., a8
// which again, is totally correct. Of course there's the case where a9 is at the end, but it doesn't
// matter for the same reasons as above.
// Addendum: We can just do the transformation for all b's, even before we care about any Jacobsthal logic.
// This way, we don't have to determine where to 'stop' transforming (i.e., the next Jacobsthal number).
// The logic stays the same. So after this function finishes, we have all the 'c' chunks (always the first 2)
// then the 'a' chunks, and then the 'b' chunks. The 'c' and 'a' chunks form the main chain.
void prepare_for_binary_insertion_vector(std::vector<int> &ints, std::size_t size, std::size_t chunk_size) {
    std::size_t chunk_idx, offset;

    // save 'b' chunks
    chunk_idx = chunk_size * 2;
    offset = 0;
    while (chunk_idx + chunk_size - 1 < size) {
        std::memmove(tmp_chunk.data() + offset, ints.data() + chunk_idx, chunk_size * sizeof(ints[0]));
        chunk_idx += chunk_size * 2;
        offset += chunk_size;
    }

    // move 'a' chunks
    chunk_idx = chunk_size * 2 + chunk_size;
    offset = chunk_size * 2;
    while (chunk_idx + chunk_size - 1 < size) {
        std::memmove(ints.data() + offset, ints.data() + chunk_idx, chunk_size * sizeof(ints[0]));
        chunk_idx += chunk_size * 2;
        offset += chunk_size;
    }

    // append 'b' chunks
    std::size_t tmp_chunk_idx = 0;
    while (offset + chunk_size - 1 < size) { // TODO refactor into a single memmove
        std::memmove(ints.data() + offset, tmp_chunk.data() + tmp_chunk_idx, chunk_size * sizeof(ints[0]));
        tmp_chunk_idx += chunk_size;
        offset += chunk_size;
    }
}

void binary_insertion_according_to_jacobsthal_numbering_vector(std::vector<int> &ints, std::size_t size, std::size_t chunk_size) {
    std::size_t prev_jacobsthal = 1;
    std::size_t jacobsthal = 3;

    // std::cout << "Preparing array for binary insertion, chunk size " << chunk_size << std::endl;
    prepare_for_binary_insertion_vector(ints, size, chunk_size);

    // std::cout << "Inserting numbers, permuted chunks now look like this" << std::endl;
    // print_vector(ints, chunk_size, true);

    while (true) {
        if (!binary_insertion_downwards_vector(ints, size, chunk_size, jacobsthal, prev_jacobsthal))
            break;
        jacobsthal += prev_jacobsthal * 2;
        prev_jacobsthal = jacobsthal - prev_jacobsthal * 2;
    }
}

void sort_main_chain_vector(std::vector<int> &ints, std::size_t size, std::size_t chunk_size) {
    if (!sort_chunk_pairs_vector(ints, size, chunk_size))
        return;
    // std::cout << "Red highlight == main chain, yellow highlight == chunk with size " << chunk_size << std::endl;
    // print_vector(ints, chunk_size, true);
    sort_main_chain_vector(ints, size, chunk_size << 1);
    binary_insertion_according_to_jacobsthal_numbering_vector(ints, size, chunk_size);
}

void ford_johnson_vector(std::vector<int> &ints, std::size_t size) {
    tmp_chunk.reserve(size);
    sort_main_chain_vector(ints, size, 1);
}
/// vector end ///

/// list start ///
std::size_t number_of_comparisons_list;

void print_list(std::list<int> &ints, std::size_t chunk_size, bool color) {
    std::list<int>::const_iterator it = ints.begin();
    for (std::size_t idx = 0; idx < ints.size(); ++idx) {
        if (idx > 0)
            std::cout << ", ";

        if (!color) {
            std::cout << *it++;
            continue;
        }

        bool is_main_chain = ((idx + 1) & (chunk_size * 2 - 1)) == 0;
        bool is_end_of_chunk = ((idx + 1) & (chunk_size - 1)) == 0;

        if (is_main_chain)
            std::cout << "\033[101;30;1;4m";
        else if (is_end_of_chunk)
            std::cout << "\033[103;30;4m";
        else
            std::cout << "\033[103;30m";
        std::cout << *it++;
        if (is_main_chain || is_end_of_chunk)
            std::cout << "\033[m";
    }
    if (color)
        std::cout << "\033[m";
    std::cout << std::endl;
}

void binary_insertion_according_to_jacobsthal_numbering_list(std::list<int> &ints, std::size_t size, std::size_t chunk_size) {
    std::size_t prev_jacobsthal = 1;
    std::size_t jacobsthal = 3;

    // std::cout << "Preparing array for binary insertion, chunk size " << chunk_size << std::endl;
    prepare_for_binary_insertion_vector(ints, size, chunk_size);

    // std::cout << "Inserting numbers, permuted chunks now look like this" << std::endl;
    // print_vector(ints, chunk_size, true);

    while (true) {
        if (!binary_insertion_downwards_vector(ints, size, chunk_size, jacobsthal, prev_jacobsthal))
            break;
        jacobsthal += prev_jacobsthal * 2;
        prev_jacobsthal = jacobsthal - prev_jacobsthal * 2;
    }
}

void ford_johnson_list(std::list<int> &ints, std::size_t size) {
    if (!sort_chunk_pairs_list(ints, size, chunk_size))
        return;
    // std::cout << "Red highlight == main chain, yellow highlight == chunk with size " << chunk_size << std::endl;
    // print_list(ints, chunk_size, true);
    sort_main_chain_list(ints, size, chunk_size << 1);
    binary_insertion_according_to_jacobsthal_numbering_list(ints, size, chunk_size);
}
/// list end ///

int main(int argc, char **argv) {

    std::vector<int> ints_vector(argc - 1);
    for (int i = 1; i < argc; ++i) {
        ints_vector[i - 1] = std::atoi(argv[i]);
    }

    std::list<int> ints_list;
    for (int i = 1; i < argc; ++i) {
        ints_list.push_back(std::atoi(argv[i]));
    }

    // std::vector<int> ints_vector = {18, 1, 5, 7, 8, 15, 12, 10, 21, 4, 20, 11, 9, 2, 6, 17, 16, 13, 3, 14, 19};
    // std::vector<int> ints_list = {18, 1, 5, 7, 8, 15, 12, 10, 21, 4, 20, 11, 9, 2, 6, 17, 16, 13, 3, 14, 19};

    std::cout << "Before (std::vector): ";
    print_vector(ints_vector, 0, false);
    std::cout << "Before (std::list):   ";
    print_list(ints_list, 0, false);

    struct timespec start_vector, end_vector;
    clock_gettime(CLOCK_PROCESS_CPUTIME_ID, &start_vector);
    ford_johnson_vector(ints_vector, ints_vector.size());
    clock_gettime(CLOCK_PROCESS_CPUTIME_ID, &end_vector);

    struct timespec start_list, end_list;
    clock_gettime(CLOCK_PROCESS_CPUTIME_ID, &start_list);
    ford_johnson_list(ints_list, ints_list.size());
    clock_gettime(CLOCK_PROCESS_CPUTIME_ID, &end_list);

    std::cout << "After (std::vector):  ";
    print_vector(ints_vector, 0, false);
    std::cout << "After (std::list):    ";
    print_list(ints_list, 0, false);

    std::cout << "Time to process a range of " << argc - 1 << " integers with std::vector: " << (end_vector.tv_sec - start_vector.tv_sec) * 1000000000 + end_vector.tv_nsec - start_vector.tv_nsec
              << " ns == " << (end_vector.tv_sec - start_vector.tv_sec) * 1000000 + (end_vector.tv_nsec - start_vector.tv_nsec) / 1000 << " us == " << (end_vector.tv_sec - start_vector.tv_sec) * 1000 + (end_vector.tv_nsec - start_vector.tv_nsec) / 1000000
              << " ms == " << end_vector.tv_sec - start_vector.tv_sec + (end_vector.tv_nsec - start_vector.tv_nsec) / 1000000000 << " s" << std::endl;
    std::cout << "Number of comparisons (std::vector): " << number_of_comparisons_vector << std::endl;

    std::cout << "Time to process a range of " << argc - 1 << " integers with std::list:   " << (end_list.tv_sec - start_list.tv_sec) * 1000000000 + end_list.tv_nsec - start_list.tv_nsec
              << " ns == " << (end_list.tv_sec - start_list.tv_sec) * 1000000 + (end_list.tv_nsec - start_list.tv_nsec) / 1000 << " us == " << (end_list.tv_sec - start_list.tv_sec) * 1000 + (end_list.tv_nsec - start_list.tv_nsec) / 1000000
              << " ms == " << end_list.tv_sec - start_list.tv_sec + (end_list.tv_nsec - start_list.tv_nsec) / 1000000000 << " s" << std::endl;
    std::cout << "Number of comparisons (std::list):   " << number_of_comparisons_list << std::endl;

    return 0;
}
