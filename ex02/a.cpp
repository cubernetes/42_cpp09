#include <cstddef>
#include <cstring>
#include <iostream>
#include <limits>

std::size_t number_of_comparisons;
int *tmp_chunk;

void print_array(int ints[], std::size_t size, std::size_t chunk_size, bool color) {
    for (std::size_t idx = 0; idx < size; ++idx) {
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
int chunk_to_number(int ints[], std::size_t idx, std::size_t chunk_size) { return ints[idx + chunk_size - 1]; }

// ascending
// assumes bound check has already been made, might segfault otherwise
void sort_chunk_pair(int ints[], std::size_t size, std::size_t idx, std::size_t chunk_size) {
    // ignore straggler chunk
    if (idx + chunk_size * 2 - 1 >= size)
	    return;

    ++number_of_comparisons;

    // biggest element is always farthest to the right
    std::size_t biggest_element_in_first_chunk = chunk_to_number(ints, idx, chunk_size);
    std::size_t biggest_element_in_second_chunk = chunk_to_number(ints, idx + chunk_size, chunk_size);

    // already sorted
    if (biggest_element_in_first_chunk < biggest_element_in_second_chunk)
        return;

    std::memmove(tmp_chunk, ints + idx, chunk_size * sizeof(*ints));
    std::memmove(ints + idx, ints + idx + chunk_size, chunk_size * sizeof(*ints));
    std::memmove(ints + idx + chunk_size, tmp_chunk, chunk_size * sizeof(*ints));
}

// returning `false` is the base case: we only have 1 or 0 elements, which are already, necessarily sorted
bool sort_chunk_pairs(int ints[], std::size_t size, std::size_t chunk_size) {
    if (chunk_size * 2 > size)
        return false; // need at least one chunk pair because the loop runs at least once
    for (std::size_t idx = 0; idx < size; idx += chunk_size * 2)
        sort_chunk_pair(ints, size, idx, chunk_size);
    return true;
}

bool binary_insert(int ints[], std::size_t size, std::size_t insert_chunk_idx, std::size_t chunk_size, std::size_t binary_search_end_chunk) {
    if (insert_chunk_idx + chunk_size - 1 >= size) {
        // out of bounds
        // std::cout << "Binary search would access index " << insert_chunk_idx + chunk_size - 1 << " (Jacobsthal logic) but we only have " << size << " numbers. Skipping" << std::endl;
        return false;
    }
    int number_to_insert = chunk_to_number(ints, insert_chunk_idx, chunk_size);
    std::cout << "Number to insert: " << number_to_insert << std::endl;

    // binary search
    std::size_t lo = 0;
    std::size_t hi = binary_search_end_chunk;
    std::size_t mi;
    int mi_num;

    while (lo < hi) {
        mi = (lo + hi) / chunk_size / 2 * chunk_size; // floor down to lower chunk
        mi_num = chunk_to_number(ints, mi, chunk_size);
        if (mi_num <= number_to_insert)
            lo = mi + chunk_size; // exclude left array from search
        else if (mi >= chunk_size)
            hi = mi - chunk_size; // exclude right array from search
        else
            hi = 0;
    }
    int lo_num = chunk_to_number(ints, lo, chunk_size);
    std::size_t insert_before_idx = lo_num < number_to_insert ? lo + chunk_size : lo;

    // insert
    // std::cout << "Insert before idx: " << insert_before_idx << std::endl;
    // std::cout << "Insert before num: " << chunk_to_number(ints, insert_before_idx, chunk_size) << std::endl;
    std::size_t shift_chunks = (insert_chunk_idx - insert_before_idx) / chunk_size;
    std::memmove(tmp_chunk, ints + insert_before_idx, shift_chunks * chunk_size * sizeof(*ints));
    std::memmove(ints + insert_before_idx, ints + insert_chunk_idx, chunk_size * sizeof(*ints));
    std::memmove(ints + insert_before_idx + chunk_size, tmp_chunk, shift_chunks * chunk_size * sizeof(*ints));
    std::cout << "After insertion" << std::endl;
    print_array(ints, size, chunk_size, true);

    return true;
}

bool binary_insertion_downwards(int ints[], std::size_t size, std::size_t chunk_size, std::size_t jacobsthal, std::size_t prev_jacobsthal, bool has_straggler) {
    --jacobsthal; // decrementing since `ints` is 0-indexed
    --prev_jacobsthal;

    if (jacobsthal >= size)
        jacobsthal = size - 1;

    bool did_something = false;
    std::size_t chunk_idx = chunk_size * (size / chunk_size / 2 + jacobsthal);
    std::size_t binary_search_end_chunk = chunk_size * (prev_jacobsthal + jacobsthal + 3);
    //if (has_straggler) {
    //    chunk_idx -= chunk_size;
    //    binary_search_end_chunk -= chunk_size;
    //}
    for (std::size_t jacobs_idx = jacobsthal; jacobs_idx > prev_jacobsthal; --jacobs_idx) {
        if (binary_insert(ints, size, chunk_idx, chunk_size, binary_search_end_chunk))
            did_something = true;
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
bool prepare_for_binary_insertion(int ints[], std::size_t size, std::size_t chunk_size) {
    std::size_t chunk_idx, offset;

    // save 'b' chunks
    chunk_idx = chunk_size * 2;
    offset = 0;
    std::size_t number_of_b_chunks = 0;
    while (chunk_idx + chunk_size - 1 < size) {
        std::memmove(tmp_chunk + offset, ints + chunk_idx, chunk_size * sizeof(*ints));
        chunk_idx += chunk_size * 2;
        offset += chunk_size;
        ++number_of_b_chunks;
    }

    // move 'a' chunks
    chunk_idx = chunk_size * 2 + chunk_size;
    offset = chunk_size * 2;
    std::size_t number_of_a_chunks = 0;
    while (chunk_idx + chunk_size - 1 < size) {
        std::memmove(ints + offset, ints + chunk_idx, chunk_size * sizeof(*ints));
        chunk_idx += chunk_size * 2;
        offset += chunk_size;
        ++number_of_a_chunks;
    }

    // append 'b' chunks
    std::size_t tmp_chunk_idx = 0;
    while (offset + chunk_size - 1 < size) { // TODO refactor into a single memmove
        std::memmove(ints + offset, tmp_chunk + tmp_chunk_idx, chunk_size * sizeof(*ints));
        tmp_chunk_idx += chunk_size;
        offset += chunk_size;
    }
    return number_of_b_chunks != number_of_a_chunks;
}

void binary_insertion_according_to_jacobsthal_numbering(int ints[], std::size_t size, std::size_t chunk_size) {
    std::size_t prev_jacobsthal = 1;
    std::size_t jacobsthal = 3;

    std::cout << "Preparing array for binary insertion" << std::endl;
    bool has_straggler = prepare_for_binary_insertion(ints, size, chunk_size);
    print_array(ints, size, chunk_size, true);

    std::cout << "Inserting numbers with chunk size " << chunk_size << std::endl;
    while (true) {
        if (!binary_insertion_downwards(ints, size, chunk_size, jacobsthal, prev_jacobsthal, has_straggler)) {
            std::cout << "Binary insertion did nothing, chunk size is " << chunk_size << std::endl;
            break;
        }
        jacobsthal += prev_jacobsthal * 2;
        prev_jacobsthal = jacobsthal - prev_jacobsthal * 2;
    }
}

void sort_main_chain(int ints[], std::size_t size, std::size_t chunk_size) {
    if (!sort_chunk_pairs(ints, size, chunk_size))
        return;
    std::cout << "Red highlight == main chain, yellow highlight == chunk with size " << chunk_size << std::endl;
    print_array(ints, size, chunk_size, true);
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
    print_array(ints, sizeof(ints) / sizeof(*ints), std::numeric_limits<std::size_t>::max(), false);
    ford_johnson(ints, sizeof(ints) / sizeof(*ints));
    std::cout << "FINAL ARRAY" << std::endl;
    print_array(ints, sizeof(ints) / sizeof(*ints), std::numeric_limits<std::size_t>::max(), false);
    std::cout << "Number of comparisons: " << number_of_comparisons << std::endl;
    return 0;
}
