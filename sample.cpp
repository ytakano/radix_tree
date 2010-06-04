#define DEBUG

#include "radix_tree.hpp"

int
main(int argc, char *argv)
{
        radix_tree<std::string, int> tree;

        tree.insert(std::pair<std::string, int>("aah", 1));
        tree.insert(std::pair<std::string, int>("aabcd", 2));
        tree.insert(std::pair<std::string, int>("eea", 3));
        tree.insert(std::pair<std::string, int>("eeffa", 4));
        tree.insert(std::pair<std::string, int>("fbb", 5));
        tree.insert(std::pair<std::string, int>("aba", 6));
        tree.insert(std::pair<std::string, int>("a", 7));

        tree["ddeee"] = 8;

        tree.print_nodes();

        tree.erase("aah");
        tree.erase("fbb");
        tree["ddeee"] = 9;

        tree.print_nodes();

        std::cout << "begin: " << (*tree.begin()).first << std::endl;

        return 0;
}
