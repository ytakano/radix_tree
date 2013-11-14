#include <string>
#include <iostream>
#include <cstdlib>

#include "../radix_tree.hpp"

radix_tree<std::string, int> tree;

void insert() {
    tree["apache"]    = 0;
    tree["afford"]    = 1;
    tree["available"] = 2;
    tree["affair"]    = 3;
    tree["avenger"]   = 4;
    tree["binary"]    = 5;
    tree["bind"]      = 6;
    tree["brother"]   = 7;
    tree["brace"]     = 8;
    tree["blind"]     = 9;
    tree["bro"]       = 10;
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

int main()
{
    insert();

    longest_match("binder");
    longest_match("bracelet");
    longest_match("apple");

    prefix_match("aff");
    prefix_match("bi");
    prefix_match("a");

    greedy_match("avoid");
    greedy_match("bring");
    greedy_match("attack");

    traverse();

    tree.erase("bro");
    prefix_match("bro");

    return EXIT_SUCCESS;
}
