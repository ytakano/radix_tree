#ifndef RADIX_TREE_HPP
#define RADIX_TREE_HPP

#include <cassert>
#include <string>
#include <utility>
#include <vector>

#ifdef DEBUG
#include <iostream>
#endif

#include "radix_tree_it.hpp"
#include "radix_tree_node.hpp"


std::string
radix_substr(const std::string &str, int begin, int num)
{
        return str.substr(begin, num);
}

std::string
radix_join(const std::string &str1, const std::string &str2)
{
        return str1 + str2;
}

bool
radix_cmp(const std::string &str1, const std::string &str2)
{
        return str1 == str2;
}

int
radix_length(const std::string &str) {
        return str.size();
}

template <typename K, typename T>
class radix_tree {
public:
        typedef K key_type;
        typedef T mapped_type;
        typedef std::pair<const K, T> value_type;
        typedef radix_tree_it<K, T>   iterator;
        typedef std::size_t           size_type;


        radix_tree() : m_size(0), m_root(NULL) { }
        ~radix_tree() { if (m_root != NULL) delete m_root; }

        size_type size()  const { return m_size; }
        bool      empty() const { return m_size == 0; }
        void      clear() { delete m_root; m_root = NULL; m_size = 0; }

        iterator find(const K &key);
        iterator begin();
        iterator end();

        std::pair<iterator, bool> insert(const value_type &val);
        size_type                 erase(const K &key);
        void                      greedy_match(const K &key,
                                               std::vector<iterator> &vec);
        iterator                  longest_match(const K &key);


        T& operator[] (const K &lhs);

private:
        size_type m_size;
        radix_tree_node<K, T>* m_root;

        radix_tree_node<K, T>* begin(radix_tree_node<K, T> *node);
        radix_tree_node<K, T>* find_node(const K &key,
                                         radix_tree_node<K, T> *node,
                                         int depth);
        radix_tree_node<K, T>* append(radix_tree_node<K, T> *parent,
                                      const value_type &val);
        radix_tree_node<K, T>* prepend(radix_tree_node<K, T> *node,
                                       const value_type &val);
        void                   greedy_match(radix_tree_node<K, T> *node,
                                            std::vector<iterator> &vec);


#ifdef DEBUG
public:
        void print_nodes();
private:
        void print_nodes(radix_tree_node<K, T> *node, std::string space);
#endif
};


#ifdef DEBUG
template <typename K, typename T>
void
radix_tree<K, T>::print_nodes()
{
        if (m_root == NULL)
                return;

        std::cout << "$root" << std::endl;
        print_nodes(m_root, "    ");
}

template <typename K, typename T>
void
radix_tree<K, T>::print_nodes(radix_tree_node<K, T> *node, std::string space)
{
        typename std::map<K, radix_tree_node<K, T>*>::iterator it;

        for (it = node->m_children.begin(); it != node->m_children.end();
             ++it) {
                if (it->first.size() > 0) {
                        std::cout << space << it->first << std::endl;
                } else {
                        std::cout << space << "$end, "
                                  << it->second->m_value.second << std::endl;
                }

                print_nodes(it->second, space + "    ");
        }
}
#endif

template <typename K, typename T>
typename radix_tree<K, T>::iterator
radix_tree<K, T>::longest_match(const K &key)
{
        if (m_root == NULL)
                return iterator(NULL);

        radix_tree_node<K, T> *node;
        K key_sub;

        node = find_node(key, m_root, 0);

        if (node->m_is_leaf)
                return iterator(node);

        key_sub = radix_substr(key, node->m_depth, radix_length(node->m_key));

        if (key_sub != node->m_key)
                node = node->m_parent;

        K nul = radix_substr(key, 0, 0);

        while (node != NULL) {
                typename radix_tree_node<K, T>::it_child it;
                it = node->m_children.find(nul);
                if (it != node->m_children.end() && it->m_is_leaf)
                        return iterator(it->second);

                node = node->m_parent;
        }

        return iterator(NULL);
}


template <typename K, typename T>
typename radix_tree<K, T>::iterator
radix_tree<K, T>::end()
{
        return iterator(NULL);
}

template <typename K, typename T>
typename radix_tree<K, T>::iterator
radix_tree<K, T>::begin()
{
        radix_tree_node<K, T> *node;

        if (m_root == NULL)
                node = NULL;
        else
                node = begin(m_root);

        return iterator(node);
}

template <typename K, typename T>
radix_tree_node<K, T>*
radix_tree<K, T>::begin(radix_tree_node<K, T> *node)
{
        if (node->m_is_leaf)
                return node;

        assert(node->m_children.size() > 0);

        return begin(node->m_children.begin()->second);
}

template <typename K, typename T>
T&
radix_tree<K, T>::operator[] (const K &lhs)
{
        iterator it = find(lhs);

        if (it == end()) {
                T t;
                std::pair<iterator, bool> ret;
                ret = insert(std::pair<K, T>(lhs, t));

                assert(ret.second == true);

                it = ret.first;
        }

        return it->second;
}

template <typename K, typename T>
void
radix_tree<K, T>::greedy_match(const K &key, std::vector<iterator> &vec)
{
        radix_tree_node<K, T> *node;

        if (m_root == NULL)
                return;

        node = find_node(key, m_root, 0);

        vec.clear();

        greedy_match(node, vec);
}

template <typename K, typename T>
void
radix_tree<K, T>::greedy_match(radix_tree_node<K, T> *node,
                               std::vector<iterator> &vec)
{
        if (node->m_is_leaf) {
                vec.push_back(iterator(node->m_value));
                return;
        }

        typename std::map<K, radix_tree_node<K, T>*>::iterator it;

        for (it = node->m_children.begin; it != node->m_children.end(); ++it) {
                greedy_match(it->second, vec);
        }
}

template <typename K, typename T>
typename radix_tree<K, T>::size_type
radix_tree<K, T>::erase(const K &key)
{
        if (m_root == NULL)
                return 0;

        radix_tree_node<K, T> *child;
        radix_tree_node<K, T> *parent;
        radix_tree_node<K, T> *grandparent;
        K nul = radix_substr(key, 0, 0);

        child = find_node(key, m_root, 0);

        if (! child->m_is_leaf)
                return 0;

        parent = child->m_parent;
        parent->m_children.erase(nul);

        delete child;

        if (parent == m_root)
                return 1;

        grandparent = parent->m_parent;
        grandparent->m_children.erase(parent->m_key);

        delete parent;

        if (grandparent == m_root) {
                return 1;
        }

        if (grandparent->m_children.size() == 1) {
                // merge grandparent with the uncle
                typename std::map<K, radix_tree_node<K, T>*>::iterator it;
                it = grandparent->m_children.begin();

                radix_tree_node<K, T> *uncle = it->second;

                uncle->m_depth = parent->m_depth;
                uncle->m_key   = radix_join(grandparent->m_key, uncle->m_key);
                uncle->m_parent = grandparent->m_parent;

                grandparent->m_children.erase(it);

                grandparent->m_parent->m_children.erase(grandparent->m_key);
                grandparent->m_parent->m_children[uncle->m_key] = uncle;

                delete grandparent;
        }

        return 1;
}


template <typename K, typename T>
radix_tree_node<K, T>*
radix_tree<K, T>::append(radix_tree_node<K, T> *parent, const value_type &val)
{
        int depth;
        int len;
        K   nul = radix_substr(val.first, 0, 0);
        radix_tree_node<K, T> *node_c, *node_cc;

        depth = parent->m_depth + radix_length(parent->m_key);
        len   = radix_length(val.first) - depth;

        if (len == 0) {
                node_c = new radix_tree_node<K, T>(val);

                node_c->m_depth   = depth;
                node_c->m_parent  = parent;
                node_c->m_key     = nul;
                node_c->m_is_leaf = true;

                parent->m_children[nul] = node_c;

                return node_c;
        } else {
                node_c = new radix_tree_node<K, T>(val);

                K key_sub = radix_substr(val.first, depth, len);

                parent->m_children[key_sub] = node_c;

                node_c->m_depth  = depth;
                node_c->m_parent = parent;
                node_c->m_key    = key_sub;


                node_cc = new radix_tree_node<K, T>(val);
                node_c->m_children[nul] = node_cc;

                node_cc->m_depth   = depth + len;
                node_cc->m_parent  = node_c;
                node_cc->m_key     = nul;
                node_cc->m_is_leaf = true;

                return node_cc;
        }
}

template <typename K, typename T>
radix_tree_node<K, T>*
radix_tree<K, T>::prepend(radix_tree_node<K, T> *node, const value_type &val)
{
        int count;
        int len1, len2;

        len1 = radix_length(node->m_key);
        len2 = radix_length(val.first) - node->m_depth;

        for (count = 0; count < len1 && count < len2; count++) {
                if (node->m_key[count] !=
                    val.first[count + node->m_depth])
                        break;
        }

        assert(count != 0);


        node->m_parent->m_children.erase(node->m_key);


        radix_tree_node<K, T> *node_a = new radix_tree_node<K, T>;

        node_a->m_parent = node->m_parent;
        node_a->m_key    = radix_substr(node->m_key, 0, count);
        node_a->m_depth  = node->m_depth;
        node_a->m_parent->m_children[node_a->m_key] = node_a;


        node->m_depth  += count;
        node->m_parent  = node_a;
        node->m_key     = radix_substr(node->m_key, count, len1 - count);
        node->m_parent->m_children[node->m_key] = node;


        K nul = radix_substr(val.first, 0, 0);
        if (count == len2) {
                radix_tree_node<K, T> *node_b;

                node_b = new radix_tree_node<K, T>(val);

                node_b->m_parent  = node_a;
                node_b->m_key     = nul;
                node_b->m_depth   = node_a->m_depth + count;
                node_b->m_is_leaf = true;
                node_b->m_parent->m_children[nul] = node_b;

                return node_b;
        } else {
                radix_tree_node<K, T> *node_b, *node_c;

                node_b = new radix_tree_node<K, T>;

                node_b->m_parent = node_a;
                node_b->m_depth  = node_a->m_depth + count;
                node_b->m_key    = radix_substr(val.first,
                                                node_b->m_depth,
                                                len2 - node_b->m_depth);
                node_b->m_parent->m_children[node_b->m_key] = node_b;


                node_c = new radix_tree_node<K, T>(val);

                node_c->m_parent  = node_b;
                node_c->m_depth   = radix_length(val.first);
                node_c->m_key     = nul;
                node_c->m_is_leaf = true;
                node_c->m_parent->m_children[nul] = node_c;

                return node_c;
        }
}

template <typename K, typename T>
std::pair<typename radix_tree<K, T>::iterator, bool>
radix_tree<K, T>::insert(const value_type &val)
{
        if (m_root == NULL) {
                K nul = radix_substr(val.first, 0, 0);

                m_root = new radix_tree_node<K, T>;
                m_root->m_key = nul;
        }

        m_size++;

        radix_tree_node<K, T> *node = find_node(val.first, m_root, 0);

        if (node->m_is_leaf) {
                node->m_value.second = val.second;
                return std::pair<iterator, bool>(node, true);
        } else if (node == m_root) {
                return std::pair<iterator, bool>(append(m_root, val), true);
        } else {
                int len     = radix_length(node->m_key);
                K   key_sub = radix_substr(val.first, node->m_depth, len);

                if (key_sub == node->m_key) {
                        return std::pair<iterator, bool>(append(node, val),
                                                         true);
                } else {
                        return std::pair<iterator, bool>(prepend(node, val),
                                                         true);
                }
        }
}

template <typename K, typename T>
typename radix_tree<K, T>::iterator
radix_tree<K, T>::find(const K &key)
{
        if (m_root == NULL)
                return iterator(NULL);

        radix_tree_node<K, T> *node = find_node(key, m_root, 0);

        // if the node is a internal node, return NULL
        if (! node->m_is_leaf)
                return iterator(NULL);

        return iterator(node);
}

template <typename K, typename T>
radix_tree_node<K, T>*
radix_tree<K, T>::find_node(const K &key, radix_tree_node<K, T> *node,
                            int depth)
{
        if (node->m_children.size() == 0)
                return node;

        typename radix_tree_node<K, T>::it_child it;
        int len_key = radix_length(key) - depth;

        for (it = node->m_children.begin(); it != node->m_children.end();
             ++it) {
                if (len_key == 0) {
                        if (it->second->m_is_leaf)
                                return it->second;
                        else
                                continue;
                }

                if (! it->second->m_is_leaf && key[depth] == it->first[0]) {
                        int len_node = radix_length(it->first);
                        K   key_sub  = radix_substr(key, depth, len_node);

                        if (radix_cmp(key_sub, it->first)) {
                                return find_node(key, it->second,
                                                 depth + len_node);
                        } else {
                                return it->second;
                        }
                }
        }

        return node;
}

/*

(root)
|
|---------------
|       |      |
abcde   bcdef  c
|   |   |      |------
|   |   $3     |  |  |
f   ge         d  e  $6
|   |          |  |
$1  $2         $4 $5

find_node():
  bcdef  -> $3
  bcdefa -> bcdef
  c      -> $6
  cf     -> c
  abch   -> abcde
  abc    -> abcde
  abcde  -> abcde
  abcdef -> $1
  abcdeh -> abcde
  de     -> (root)


(root)
|
abcd
|
$

(root)
|
$

*/

#endif // RADIX_TREE_HPP
