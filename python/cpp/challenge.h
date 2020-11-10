#include "measure.h"

#include <string.h>


using Sum = std::uint32_t;  // Type used for the track & item ints.
// Based on mask tasks observed in practice.
constexpr int MAX_ITEMS = 100000;
constexpr int MAX_TRACK = 10000;
// Note: we keep everything as a flat structure, but item `2k` is `start`, `2k+1` is `end`.
using Items = std::array<Sum, 2*MAX_ITEMS>;  // *2 because we store start,end in sequence
using Track = std::array<Sum, MAX_TRACK+1>;  // +1 because for the running sum we have the start '0'
using Outputs = std::array<Sum, MAX_ITEMS>;


// ============
// PARSING JSON
// ============
// Take in e.g. {"items":[[0,1],[2,4]],"track":[13,16,12,3,18,5,2,8,1,14]} and parse ints.
// Takes care of producing the running sum.
void parse_data(std::string_view data, Items& items, Items::size_type& item_count,
                Track& running_sum, Track::size_type& track_count);
// Reads a number and moves past it.
Sum read_number(std::string_view& data);

// =======
// SOLVING
// =======
// Solves the given 'items' queries on a precomputed 'running_sum' track.
// Supports solving a subset e.g. to run multithreaded.
void solve_subset(const Track& running_sum, const Items& items,
                  Outputs& outputs, Outputs::size_type start, Outputs::size_type end);

// ==============
// OUTPUTING JSON
// ==============
// Produces the final output, i.e. a JSON list of ints.
char* output_sums(const Outputs& sums, const int sums_count, std::string::size_type* out_len);
// Outputs int 'n' at [ptr-digits(n), ptr). Outputs `ptr - digits(n)` (start of the output).
// i.e. if I give it:  "      ", with n=13
//                            ^ptr
// it will write "    13"
//                    ^return val
// The idea is that we can write ints faster moving backwards (don't need to know the length).
inline char* format_int_moving_back(char* ptr, Sum n);

// ==============
// MAIN CHALLENGE
// ==============
// Take in JSON, solve the problem, and output JSON.
const std::string_view seqsum(std::string_view data) {
    Track running_sum;
    Items items;
    Track::size_type track_count;
    Items::size_type items_count;
    measure("parse", [&] {
        parse_data(data, items, items_count, running_sum, track_count);
    });
    const Outputs::size_type sums_count = items_count / 2;

    static Outputs outputs;
    measure("solve", [&] {
        solve_subset(running_sum, items, outputs, 0, sums_count);
    });
    const char* out_ptr;
    std::string_view::size_type out_len;
    measure("output", [&] {
        out_ptr = output_sums(outputs, sums_count, &out_len);
    });
    return std::string_view{out_ptr, out_len};
}


char* output_sums(const Outputs& sums, const int sums_count, std::string::size_type* out_len) {
    // Upper bound: max 6 digits per sum + ',' for N-1 sums, + '[]'
    constexpr int SUM_MAX_DIGITS = 6;
    constexpr int MAX_OUTPUT_LEN = MAX_ITEMS * (SUM_MAX_DIGITS+1) + 2;
    static char output[MAX_OUTPUT_LEN];
    char* ptr = output;

    // Note: we're building the output in reverse, so that we can output digits
    // without requiring extra copies (because we don't want to compute how many
    // digits there are for each int before starting to copy).
    ptr += MAX_OUTPUT_LEN;
    *--ptr = '\0';  // not strictly necessary, but nice to debug.
    *--ptr = ']';
    ptr = format_int_moving_back(ptr, sums[sums_count - 1]);
    for (int i = static_cast<int>(sums_count - 2); i >= 0; --i) {
        *--ptr = ',';
        ptr = format_int_moving_back(ptr, sums[i]);
    }
    *--ptr = '[';
    // -1 to remove \0 in count
    *out_len = static_cast<int>(output + MAX_OUTPUT_LEN - 1 - ptr);
    return ptr;
}

char* format_int_moving_back(char* ptr, Sum n) {
    // Outputs 'n' at [ptr-digits(n), ptr). Outputs ptr-digits(n) (start of int).

    // NOTE: comes from the fmt lib, simplified/inlined for my needs:
    // https://github.com/fmtlib/fmt/blob/a30b279bad752ff4c4967c6d1bfcfc8d79c6b170/include/fmt/format.h#L1052
    // We use this based on https://www.zverovich.net/2013/09/07/integer-to-string-conversion-in-cplusplus.html
    using digit_pair = char[2];
    static const digit_pair digit_pairs[] = {
    {'0', '0'}, {'0', '1'}, {'0', '2'}, {'0', '3'}, {'0', '4'}, {'0', '5'},
    {'0', '6'}, {'0', '7'}, {'0', '8'}, {'0', '9'}, {'1', '0'}, {'1', '1'},
    {'1', '2'}, {'1', '3'}, {'1', '4'}, {'1', '5'}, {'1', '6'}, {'1', '7'},
    {'1', '8'}, {'1', '9'}, {'2', '0'}, {'2', '1'}, {'2', '2'}, {'2', '3'},
    {'2', '4'}, {'2', '5'}, {'2', '6'}, {'2', '7'}, {'2', '8'}, {'2', '9'},
    {'3', '0'}, {'3', '1'}, {'3', '2'}, {'3', '3'}, {'3', '4'}, {'3', '5'},
    {'3', '6'}, {'3', '7'}, {'3', '8'}, {'3', '9'}, {'4', '0'}, {'4', '1'},
    {'4', '2'}, {'4', '3'}, {'4', '4'}, {'4', '5'}, {'4', '6'}, {'4', '7'},
    {'4', '8'}, {'4', '9'}, {'5', '0'}, {'5', '1'}, {'5', '2'}, {'5', '3'},
    {'5', '4'}, {'5', '5'}, {'5', '6'}, {'5', '7'}, {'5', '8'}, {'5', '9'},
    {'6', '0'}, {'6', '1'}, {'6', '2'}, {'6', '3'}, {'6', '4'}, {'6', '5'},
    {'6', '6'}, {'6', '7'}, {'6', '8'}, {'6', '9'}, {'7', '0'}, {'7', '1'},
    {'7', '2'}, {'7', '3'}, {'7', '4'}, {'7', '5'}, {'7', '6'}, {'7', '7'},
    {'7', '8'}, {'7', '9'}, {'8', '0'}, {'8', '1'}, {'8', '2'}, {'8', '3'},
    {'8', '4'}, {'8', '5'}, {'8', '6'}, {'8', '7'}, {'8', '8'}, {'8', '9'},
    {'9', '0'}, {'9', '1'}, {'9', '2'}, {'9', '3'}, {'9', '4'}, {'9', '5'},
    {'9', '6'}, {'9', '7'}, {'9', '8'}, {'9', '9'}};
    while (n >= 100) {
        // Integer division is slow so do it for a group of two digits instead
        // of for every digit. The idea comes from the talk by Alexandrescu
        // "Three Optimization Tips for C++". See speed-test for a comparison.
        ptr -= 2;
        memcpy(ptr, digit_pairs[n % 100], 2);
        n /= 100;
    }
    if (n < 10) {
        *--ptr = static_cast<char>('0' + n);
        return ptr;
    }
    ptr -= 2;
    memcpy(ptr, digit_pairs[n], 2);
    return ptr;
}

void solve_subset(const Track& running_sum, const Items& items,
                  Outputs& outputs, Outputs::size_type start, Outputs::size_type end) {
    Outputs::size_type i = start;
    for (Items::size_type item_i = start * 2; item_i < end * 2; item_i += 2) {
        const Sum query_start = std::min(items[item_i], items[item_i+1]);
        const Sum query_end = std::max(items[item_i], items[item_i+1]);
        const Sum sum = running_sum[query_end] - running_sum[query_start];
        outputs[i] = sum;
        ++i;
    } 
}

void parse_data(std::string_view data, Items& items, Items::size_type& item_count,
                Track& running_sum, Track::size_type& track_count) {
    // E.g. {"items":[[0,1],[2,4]],"track":[13,16,12,3,18,5,2,8,1,14]}

    // TODO parse from the back, using fixed lengths (instead of for-loop in read_number)?
    // see https://tombarta.wordpress.com/2008/04/23/specializing-atoi/
    // Tried.. didn't help though...

    measure("parse::items", [&] {
        data.remove_prefix(data.find(":[[") + 3);
        item_count = 0;
        while (data.front() != '"') {
            items[item_count] = read_number(data);
            data.remove_prefix(1);  // skip ','
            items[item_count + 1] = read_number(data);
            data.remove_prefix(3);  // skip '],['
            // Note: on the last one, we'll actually skip ']],', so the next one will be '"'.
            item_count += 2;
        }
    });

    measure("parse::track", [&] {
        data.remove_prefix(data.find(":[") + 2);
        // Store the track directly as a 'runnin_sum', as we read it.
        Sum sum_so_far = 0;
        running_sum[0] = 0;
        track_count = 1; // store at '1', because we'll storing a running_sum
        while (data.front() != '}') {
            sum_so_far += read_number(data);
            running_sum[track_count++] = sum_so_far;
            data.remove_prefix(1);  // skip ','
            // Note: on the last one, we'll actually skip ']', so the next one will be '}'
        }
        --track_count; // because we started at 1, remove 1 for the real count.
    });
}

Sum read_number(std::string_view& data) {
    Sum n = 0;
    for (int i = 0; i < 5; ++i) {  // max 5 digits in practice
        int digit = static_cast<int>(data.front()) - static_cast<int>('0');
        if (digit < 0 || digit > 9) break;
        n = n * 10 + digit;
        data.remove_prefix(1);
    }
    return n;
}