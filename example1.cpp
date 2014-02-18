#include <string>
#include <iostream>

#include "radix_tree.hpp"

using namespace std;

radix_tree<string, int> tree;

void
insert()
{
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
}

void
longest_match(string key)
{
        radix_tree<string, int>::iterator it;

        it = tree.longest_match(key);

        cout << "longest_match(\"" << key << "\"):" << endl;

        if (it != tree.end()) {
                cout << "    " << it->first << ", " << it->second << endl;
        } else {
                cout << "    failed" << endl;
        }
}

void
prefix_match(string key)
{
        vector<radix_tree<string, int>::iterator> vec;
        vector<radix_tree<string, int>::iterator>::iterator it;

        tree.prefix_match(key, vec);

        cout << "prefix_match(\"" << key << "\"):" << endl;

        for (it = vec.begin(); it != vec.end(); ++it) {
                cout << "    " << (*it)->first
                     << ", " << (*it)->second
                     << endl;
        }
}

void
greedy_match(string key)
{
        vector<radix_tree<string, int>::iterator> vec;
        vector<radix_tree<string, int>::iterator>::iterator it;

        tree.greedy_match(key, vec);

        cout << "greedy_match(\"" << key << "\"):" << endl;

        for (it = vec.begin(); it != vec.end(); ++it) {
                cout << "    " << (*it)->first
                     << ", " << (*it)->second
                     << endl;
        }
}

void
traverse()
{
        radix_tree<string, int>::iterator it;

        cout << "traverse:" << endl;
        for (it = tree.begin(); it != tree.end(); ++it) {
                cout << "    " << it->first << ", " << it->second << endl;
        }
}

int
main(int argc, char **argv)
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

        return 0;
}
