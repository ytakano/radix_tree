#include "common.hpp"

TEST(insert, change_size)
{
    std::vector<std::string> unique_keys = get_unique_keys();
    for (size_t i = 0; i < unique_keys.size(); i++) {
        tree_t tree;
        { // fill tree with some data
            std::random_shuffle(unique_keys.begin(), unique_keys.end());
            for (size_t j = 0; j < unique_keys.size(); j++) {
                const std::string key = unique_keys[j];
                ASSERT_EQ(j, tree.size());
                tree.insert( tree_t::value_type(key, rand()%100) );
                ASSERT_EQ(j+1, tree.size());
            }
        }
        { // try to insert with duplicate keys
            std::random_shuffle(unique_keys.begin(), unique_keys.end());
            for (size_t j = 0; j < unique_keys.size(); j++) {
                const std::string key = unique_keys[j];
                ASSERT_EQ(unique_keys.size(), tree.size());
                tree.insert( tree_t::value_type(key, rand()%100) );
                ASSERT_EQ(unique_keys.size(), tree.size());
            }
        }
    }
}

TEST(insert, success_if_key_unique)
{
    std::vector<std::string> unique_keys = get_unique_keys();
    for (size_t i = 0; i < unique_keys.size(); i++) {
        tree_t tree;
        std::random_shuffle(unique_keys.begin(), unique_keys.end());
        for (size_t j = 0; j < unique_keys.size(); j++) {
            const std::string key = unique_keys[j];
            std::pair<tree_t::iterator, bool> r = tree.insert( tree_t::value_type(key, rand()%100) );
            ASSERT_TRUE( r.second ) << "fail to insert key=" << key;
        }
    }
}

TEST(insert, fail_if_key_duplicate)
{
    std::vector<std::string> unique_keys = get_unique_keys();
    for (size_t i = 0; i < unique_keys.size(); i++) {
        tree_t tree;
        { // fill tree with some data
            std::random_shuffle(unique_keys.begin(), unique_keys.end());
            for (size_t j = 0; j < unique_keys.size(); j++) {
                const std::string key = unique_keys[j];
                tree.insert( tree_t::value_type(key, rand()%100) );
            }
        }
        { // try to insert with duplicate keys
            std::random_shuffle(unique_keys.begin(), unique_keys.end());
            for (size_t j = 0; j < unique_keys.size(); j++) {
                const std::string key = unique_keys[j];
                std::pair<tree_t::iterator, bool> r = tree.insert( tree_t::value_type(key, rand()%100) );
                ASSERT_FALSE( r.second ) << "unexpectedly inserted key=" << key;
            }
        }
    }
}

TEST(insert, dont_replace_prev_value)
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
                tree.insert( tree_t::value_type(key, value) );
                value_map[key]=value;
            }
        }
        { // try to overwrite by key
            std::random_shuffle(unique_keys.begin(), unique_keys.end());
            for (size_t j = 0; j < unique_keys.size(); j++) {
                const std::string key = unique_keys[j];
                tree.insert( tree_t::value_type(key, rand()%100) );\
            }
        }
        { // check old data was not modified
            ASSERT_EQ(value_map.size(), tree.size());
            for (tree_t::iterator it = tree.begin(); it != tree.end(); ++it) {
                ASSERT_FALSE(value_map.find(it->first) == value_map.end()) << "there is no such key in map";
                ASSERT_EQ(value_map[it->first], it->second);
            }
        }
    }
}

TEST(insert, operator_index_call_default_ctor)
{
    std::vector<std::string> unique_keys = get_unique_keys();
    std::random_shuffle(unique_keys.begin(), unique_keys.end());
    tree_t tree;
    for (size_t i = 0; i < unique_keys.size(); i++) {
        ASSERT_EQ(0, tree[unique_keys[i]]);
    }
}

TEST(insert, after_erase)
{
    std::vector<std::string> unique_keys = get_unique_keys();
    std::random_shuffle(unique_keys.begin(), unique_keys.end());
    tree_t tree;
    for (size_t i = 0; i < unique_keys.size(); i++) {
        const std::string key = unique_keys[i];
        tree.insert( tree_t::value_type(key, rand()%100) );
        tree.erase(key);
        std::pair<tree_t::iterator, bool> r = tree.insert( tree_t::value_type(key, rand()%100) );
        ASSERT_TRUE(r.second);
    }
}
