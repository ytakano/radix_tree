#include "common.hpp"

TEST(longest_match, empty_tree)
{
    std::vector<std::string> unique_keys = get_unique_keys();
    tree_t tree;
    for (size_t i = 0; i < unique_keys.size(); i++) {
        const std::string key = unique_keys[i];
        tree_t::iterator found_it = tree.longest_match(key);
        ASSERT_EQ(found_it, tree.end());
    }
}

TEST(longest_match, complex_tree)
{
    tree_t tree;

    tree["abcdef"] = 1;
    tree["abcdege"] = 2;
    tree["bcdef"] = 3;
    tree["cd"] = 4;
    tree["ce"] = 5;
    tree["c"] = 6;

    {
        SCOPED_TRACE("longest_match should find object by existent key");
        for (tree_t::iterator it = tree.begin(); it != tree.end(); ++it) {
            tree_t::iterator found_it = tree.longest_match(it->first);
            ASSERT_NE(found_it, tree.end());
            ASSERT_EQ(tree[it->first], found_it->second);
        }
    }
    {
        SCOPED_TRACE("longest_match should find nothing by empty key");
        tree_t::iterator found_it = tree.longest_match("");
        ASSERT_EQ(found_it, tree.end());
    }
    {
        SCOPED_TRACE("longest_match corrects typos in suffix");
        typedef std::map<std::string, std::string> typos_t;
        typos_t typos;
        typos["abcdefe"] = "abcdef";
        typos["abcdegeasdf"] = "abcdege";
        typos["bcdefege"] = "bcdef";
        typos["ced"] = "ce";
        typos["cdef"] = "cd";
        typos["cf"] = "c";
        typos["ca"] = "c";
        typos["ccdef"] = "c";
        for (typos_t::const_iterator typo = typos.begin(); typo != typos.end(); ++typo) {
            SCOPED_TRACE(typo->first);
            tree_t::iterator found_it = tree.longest_match(typo->first);
            ASSERT_NE(found_it, tree.end());
            ASSERT_EQ(typo->second, found_it->first);
            ASSERT_EQ(tree[typo->second] , found_it->second);
        }
    }
    {
        SCOPED_TRACE("should never be found");
        const std::string never_found_strings[] = {
            "a", "b", "d", "e", "f", "abcde", "bcdege", "acd", "bce", "acdef"
        };
        std::vector<std::string> should_never_be_found = make_vector(never_found_strings);
        for (size_t i = 0; i < should_never_be_found.size(); i++) {
            const std::string key = should_never_be_found[i];
            SCOPED_TRACE(key);
            tree_t::iterator found_it = tree.longest_match(key);
            ASSERT_EQ(found_it, tree.end());
        }
    }
}
