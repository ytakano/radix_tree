#include <string>
#include <iostream>
#include <cstdlib>

#include "../radix_tree.hpp"

radix_tree<std::string, int> tree;

void insert() {
    tree["abc"] = 0;
    tree["ade"] = 0;
}

void longest_match(std::string key)
{
    radix_tree<std::string, int>::iterator it;

    it = tree.longest_match(key);

    std::cout << "longest_match(\"" << key << "\"):" << std::endl;

    if (it != tree.end()) {
        std::cout << "    " << it->first << ", " << it->second << std::endl;
    } else {
        std::cout << "    failed" << std::endl;
    }
}

void prefix_match(std::string key)
{
    std::vector<radix_tree<std::string, int>::iterator> vec;
    std::vector<radix_tree<std::string, int>::iterator>::iterator it;

    tree.prefix_match(key, vec);

    std::cout << "prefix_match(\"" << key << "\"):" << std::endl;

    for (it = vec.begin(); it != vec.end(); ++it) {
        std::cout << "    " << (*it)->first << ", " << (*it)->second << std::endl;
    }
}

void greedy_match(std::string key)
{
    std::vector<radix_tree<std::string, int>::iterator> vec;
    std::vector<radix_tree<std::string, int>::iterator>::iterator it;

    tree.greedy_match(key, vec);

    std::cout << "greedy_match(\"" << key << "\"):" << std::endl;

    for (it = vec.begin(); it != vec.end(); ++it) {
        std::cout << "    " << (*it)->first << ", " << (*it)->second << std::endl;
    }
}

void traverse() {
    radix_tree<std::string, int>::iterator it;

    std::cout << "traverse:" << std::endl;
    for (it = tree.begin(); it != tree.end(); ++it) {
        std::cout << "    " << it->first << ", " << it->second << std::endl;
    }
}

//#include "../tests/common.hpp"
typedef radix_tree<std::string, int> tree_t;

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
    assert( is_unique(vec.begin(), vec.end()) );
    return vec;
}

int main()
{
    /*
    tree["ab"] = 0;
    tree.print_tree();
    tree["aba"] = 1;
    tree["aaa"] = 2;
    tree.print_tree();
    tree.erase("aaa");
    tree.print_tree();
    tree["aaa"] = 2;
    tree.print_tree();
    */

    std::vector<std::string> unique_keys = get_unique_keys();
    std::random_shuffle(unique_keys.begin(), unique_keys.end());
    tree_t tree;
    for (size_t i = 0; i < unique_keys.size(); i++) {
        const std::string key = unique_keys[i];
        std::cout << "insert: " << key << std::endl;
        tree.print_tree();
        tree.insert( tree_t::value_type(key, rand()%100) );
        tree.print_tree();
        tree.erase(key);
        tree.print_tree();
        std::pair<tree_t::iterator, bool> r = tree.insert( tree_t::value_type(key, rand()%100) );
        //ASSERT_TRUE(r.second);
    }

    return EXIT_SUCCESS;
}
