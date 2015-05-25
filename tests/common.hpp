#include <gtest/gtest.h>
#include <radix_tree.hpp>

#include <algorithm>
#include <map>

// this file contains some common code for all tests to reduce the number of copypaste lines

typedef radix_tree<std::string, int> tree_t;
typedef std::vector<tree_t::iterator> vector_found_t;
typedef std::map<std::string, int> map_found_t;

template<typename _RAIter>
bool is_unique(_RAIter begin, _RAIter end) {
    std::sort(begin, end);
    _RAIter it = std::adjacent_find(begin, end);
    return it == end;
}

template< typename T, size_t N >
std::vector<T> make_vector( const T (&data)[N] )
{
    return std::vector<T>(data, data+N);
}

std::vector<std::string> get_unique_keys() {
    const std::string unique_keys_strings[] = {
        "a", "b", "ab", "ba", "aa", "bb",
        "aaa", "aab", "aba", "baa", "bba", "bab", "abb", "bbb"
    };
    std::vector<std::string> vec = make_vector(unique_keys_strings);
    EXPECT_TRUE( is_unique(vec.begin(), vec.end()) );
    return vec;
}

map_found_t vec_found_to_map(const vector_found_t& vec) {
    map_found_t result;
    for (size_t i = 0; i < vec.size(); i++) {
        tree_t::iterator it = vec[i];
        result[it->first] = it->second;
    }
    return result;
}
