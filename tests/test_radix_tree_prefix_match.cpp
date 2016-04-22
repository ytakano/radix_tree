#include "common.hpp"

bool is_prefix_of(const std::string& prefix, const std::string& str) {
    std::pair<std::string::const_iterator, std::string::const_iterator> p = std::mismatch(prefix.begin(), prefix.end(), str.begin());
    return p.first == prefix.end();
}

void check_nonexistent_prefixes(tree_t& tree)
{
    SCOPED_TRACE("should never be found");
    const std::string never_found_strings[] = {
        "abcdfe", "abcdefe", "abe", "cc", "abcdec", "bcdefc"
    };
    std::vector<std::string> should_never_be_found = make_vector(never_found_strings);
    for (size_t i = 0; i < should_never_be_found.size(); i++) {
        const std::string key = should_never_be_found[i];
        SCOPED_TRACE(key);
        vector_found_t vec;
        tree.prefix_match(key, vec);
        ASSERT_EQ(0u, vec.size());
    }
}

TEST(prefix_match, empty_tree)
{
    tree_t tree;
    check_nonexistent_prefixes(tree);
}

TEST(prefix_match, complex_tree)
{
    tree_t tree;

    tree["abcdef"] = 1;
    tree["abcdege"] = 2;
    tree["bcdef"] = 3;
    tree["cd"] = 4;
    tree["ce"] = 5;
    tree["c"] = 6;
    tree[""] = 7;

    {
        SCOPED_TRACE("prefix_match should find at least 1 object by existent key");
        for (tree_t::iterator it = tree.begin(); it != tree.end(); ++it) {
            vector_found_t vec;
            tree.prefix_match(it->first, vec);
            ASSERT_GE(vec.size(), 1u);
            map_found_t map_found = vec_found_to_map(vec);
            ASSERT_NE(map_found.end(), map_found.find(it->first)) << "there is no such key in found";
            ASSERT_EQ(map_found[it->first], it->second);
        }
    }
    {
        SCOPED_TRACE("prefix_match should find every object by empty key");
        vector_found_t vec;
        tree.prefix_match("", vec);
        map_found_t should_be_found;
        for (tree_t::iterator it = tree.begin(); it != tree.end(); ++it) {
            should_be_found[it->first] = it->second;
        }
        ASSERT_EQ(should_be_found, vec_found_to_map(vec));
    }
    {
        typedef std::map<std::string, map_found_t> prefixes_t;
        prefixes_t prefixes;
        {
            // build prefixes START
            // iterate over each key in tree, make prefixes from it: prefix = key[0..N], N <- [0 .. lenght key]
            for (tree_t::iterator it = tree.begin(); it != tree.end(); ++it) {
                const std::string key = it->first;
                for (size_t i = 0; i < key.size(); i++) {
                    const std::string prefix = key.substr(0, i);

                    if (prefixes.find(prefix) != prefixes.end())
                        continue; // we should not build prefixes if we have done it before

                    vector_found_t vec;
                    for (tree_t::iterator each_it = tree.begin(); each_it != tree.end(); ++each_it) {
                        if ( is_prefix_of(prefix, each_it->first) )
                            vec.push_back(each_it);
                    }
                    prefixes[prefix] = vec_found_to_map(vec);
                }
            }
            // build prefixes END
        }

        for (prefixes_t::const_iterator prefix_it = prefixes.begin(); prefix_it != prefixes.end(); ++prefix_it) {
            SCOPED_TRACE(prefix_it->first);
            vector_found_t vec;
            tree.prefix_match(prefix_it->first, vec);
            ASSERT_EQ(prefix_it->second, vec_found_to_map(vec));
        }
    }
    check_nonexistent_prefixes(tree);
}
