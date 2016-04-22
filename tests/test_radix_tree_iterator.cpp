#include "common.hpp"

TEST(iterator, begin_end)
{
    {
        SCOPED_TRACE("empty tree");
        tree_t tree;
        ASSERT_EQ(tree.begin(), tree.end());
    }
    {
        SCOPED_TRACE("non empty tree");
        tree_t tree;
        std::vector<std::string> unique_keys = get_unique_keys();
        { // fill tree with some data
            std::random_shuffle(unique_keys.begin(), unique_keys.end());
            for (size_t i = 0; i < unique_keys.size(); i++) {
                const std::string key = unique_keys[i];
                tree.insert( tree_t::value_type(key, rand()%100) );
            }
        }
        ASSERT_NE(tree.begin(), tree.end());
    }
}

TEST(iterator, distance)
{
    {
        SCOPED_TRACE("empty tree");
        tree_t tree;
        ASSERT_EQ(0, std::distance(tree.begin(), tree.end()) );
    }
    {
        SCOPED_TRACE("non empty tree");
        tree_t tree;
        std::vector<std::string> unique_keys = get_unique_keys();
        { // fill tree with some data
            std::random_shuffle(unique_keys.begin(), unique_keys.end());
            for (size_t i = 0; i < unique_keys.size(); i++) {
                const std::string key = unique_keys[i];
                tree.insert( tree_t::value_type(key, rand()%100) );
            }
        }
        ASSERT_EQ(unique_keys.size(), size_t(std::distance(tree.begin(), tree.end())) );
    }
}

TEST(iterator, increment)
{
    {
        SCOPED_TRACE("empty tree");
        tree_t tree;
        tree_t::iterator it = tree.begin();
        ASSERT_EQ(tree.begin(), it);
        ASSERT_EQ(tree.end(), it);
        ASSERT_NO_THROW( it++ );
        ASSERT_NO_THROW( ++it );
    }
    {
        SCOPED_TRACE("non empty tree");
        tree_t tree;
        std::vector<std::string> unique_keys = get_unique_keys();
        std::set<std::string> keys;
        { // fill tree with some data
            std::random_shuffle(unique_keys.begin(), unique_keys.end());
            for (size_t i = 0; i < unique_keys.size(); i++) {
                const std::string key = unique_keys[i];
                tree.insert( tree_t::value_type(key, rand()%100) );
                keys.insert(key);
            }
        }
        {
            SCOPED_TRACE("postincrement");
            for(tree_t::iterator it = tree.begin(); it != tree.end(); ) {
                ASSERT_NE(keys.end(), keys.find(it->first));
                tree_t::iterator copy = it++;
                ASSERT_NE(copy, it);
            }
        }
        {
            SCOPED_TRACE("preincrement");
            for(tree_t::iterator it = tree.begin(); it != tree.end(); ) {
                ASSERT_NE(keys.end(), keys.find(it->first));
                ASSERT_NE(keys.end(), keys.find((*it).first));
                tree_t::iterator copy = ++it;
                ASSERT_EQ(copy, it);
            }
        }
    }
}

TEST(iterator, std__copy)
{
    SCOPED_TRACE("non empty tree");
    tree_t tree;
    std::vector<std::string> unique_keys = get_unique_keys();
    { // fill tree with some data
        std::random_shuffle(unique_keys.begin(), unique_keys.end());
        for (size_t i = 0; i < unique_keys.size(); i++) {
            const std::string key = unique_keys[i];
            tree.insert( tree_t::value_type(key, rand()%100) );
        }
    }
    std::map<std::string, int> map;
    std::copy(tree.begin(), tree.end(), std::inserter(map, map.end()));

    ASSERT_EQ(map.size(), size_t(std::distance(tree.begin(), tree.end())) );
    for(tree_t::iterator it = tree.begin(); it != tree.end(); ++it) {
        ASSERT_NE(map.end(), map.find(it->first));
    }
}
