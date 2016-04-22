#include "common.hpp"

TEST(greedy_match, complex_tree)
{
    tree_t tree;

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

    {
        SCOPED_TRACE("greedy_match should find at least 1 object by existent key");
        for (tree_t::iterator it = tree.begin(); it != tree.end(); ++it) {
            vector_found_t vec;
            tree.greedy_match(it->first, vec);
            ASSERT_GE(vec.size(), 1u);
            map_found_t map_found = vec_found_to_map(vec);
            ASSERT_NE(map_found.end(), map_found.find(it->first)) << "there is no such key in found";
            ASSERT_EQ(map_found[it->first], it->second);
        }
    }
    {
        SCOPED_TRACE("greedy_match corrects typos");
        typedef std::map<std::string, map_found_t > typos_t;
        typos_t typos;
        {
            // build typos START
            const std::string typos_strings[] = {
                // format: "typo,FOUND_1,FOUND_2,FOUND_N"
                // if FOUND == * then FOUND treated as all keys in tree
                "apple,apache",
                "zzzzz,*",
                ",*",
                "lalalalala,*",
                "avoid,available,avenger",
                "bring,brace,bro,brother",
                "biss,binary,bind",
                "attack,affair,afford,apache,available,avenger"
            };
            std::vector<std::string> typos_vector = make_vector(typos_strings);
            for (size_t i = 0; i < typos_vector.size(); i++) {
                std::vector<std::string> elems;
                std::stringstream ss(typos_vector[i]);
                std::string item;
                while (std::getline(ss, item, ',')) {
                    elems.push_back(item);
                    item.clear();
                }
                map_found_t map_found;
                if (elems.size() == 2 && elems[1] == "*") {
                    for (tree_t::iterator it = tree.begin(); it != tree.end(); ++it) {
                        map_found[it->first] = it->second;
                    }
                } else {
                    for (size_t j = 1 /* skip the first elem*/; j < elems.size(); j++) {
                        const std::string key = elems[j];
                        map_found[key] = tree[key];
                    }
                }
                typos[ elems[0] ] = map_found;
            }
            // build typos END
        }
        
        for (typos_t::const_iterator typo = typos.begin(); typo != typos.end(); ++typo) {
            SCOPED_TRACE(typo->first);
            vector_found_t vec;
            tree.greedy_match(typo->first, vec);
            ASSERT_EQ(typo->second, vec_found_to_map(vec));
        }
    }
}
