#include "common.hpp"

TEST(find, nothing_in_empty)
{
    std::vector<std::string> unique_keys = get_unique_keys();
    tree_t tree;
    for (size_t i = 0; i < unique_keys.size(); i++) {
        const std::string key = unique_keys[i];
        tree_t::iterator it = tree.find(key);
        ASSERT_EQ(tree.end(), it);
    }
}

TEST(find, find_after_insert_or_erase)
{
    std::vector<std::string> unique_keys = get_unique_keys();
    for (size_t i = 0; i < unique_keys.size(); i++) {
        tree_t tree;
        std::map<std::string, int> value_map;
        { // fill tree with some data and save values in map
            std::random_shuffle(unique_keys.begin(), unique_keys.end());
            for (size_t j = 0; j < unique_keys.size(); j++) {
                const std::string key = unique_keys[j];
                int value = rand()%100;
                std::pair<tree_t::iterator, bool> r = tree.insert( tree_t::value_type(key, value) );
                value_map[key]=value;
            }
        }
        {
            SCOPED_TRACE("try to find existent key");
            std::random_shuffle(unique_keys.begin(), unique_keys.end());
            for (size_t j = 0; j < unique_keys.size(); j++) {
                const std::string key = unique_keys[j];
                tree_t::iterator it = tree.find(key);
                ASSERT_NE(tree.end(), it);
                ASSERT_EQ(value_map[it->first], it->second);
            }
        }
        {
            SCOPED_TRACE("find should fail to find removed key");
            for (size_t j = 0; j < unique_keys.size(); j++) {
                const std::string key = unique_keys[j];
                tree.erase(key);
                tree_t::iterator it = tree.find(key);
                ASSERT_EQ(tree.end(), it);
            }
        }
    }
}
