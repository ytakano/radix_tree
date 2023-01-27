#ifndef RADIX_TREE_HPP
#define RADIX_TREE_HPP

#include <cassert>
#include <string>
#include <utility>
#include <vector>

#include "radix_tree_it.hpp"
#include "radix_tree_node.hpp"
#include <functional>

template<typename K>
K radix_substr(const K &key, int begin, int num);

template<>
inline std::string radix_substr<std::string>(const std::string &key, int begin, int num)
{
    return key.substr(begin, num);
}

template<typename K>
K radix_join(const K &key1, const K &key2);

template<>
inline std::string radix_join<std::string>(const std::string &key1, const std::string &key2)
{
    return key1 + key2;
}

template<typename K>
int radix_length(const K &key);

template<>
inline int radix_length<std::string>(const std::string &key)
{
    return static_cast<int>(key.size());
}

template <typename K, typename T, typename Compare>
class radix_tree {
public:
    typedef K key_type;
    typedef T mapped_type;
    typedef std::pair<const K, T> value_type;
    typedef radix_tree_it<K, T, Compare>   iterator;
    typedef std::size_t           size_type;

    radix_tree() : m_size(0), m_predicate(Compare()) { }
    explicit radix_tree(Compare pred) : m_size(0), m_predicate(pred) { }
    ~radix_tree() = default;

    size_type size()  const {
        return m_size;
    }
    bool empty() const {
        return m_size == 0;
    }
    void clear() {
        m_root.reset();
        m_size = 0;
    }

    iterator find(const K &key);
    iterator begin();
    iterator end();

    std::pair<iterator, bool> insert(const value_type &val);
    bool erase(const K &key);
    void erase(iterator it);
    void prefix_match(const K &key, std::vector<iterator> &vec);
    void greedy_match(const K &key,  std::vector<iterator> &vec);
    iterator longest_match(const K &key);

    T& operator[] (const K &lhs);

	template<class _UnaryPred> void remove_if(_UnaryPred pred)
	{
		radix_tree<K, T, Compare>::iterator backIt;
		for (radix_tree<K, T, Compare>::iterator it = begin(); it != end(); it = backIt)
		{
			backIt = it;
			backIt++;
			K toDelete = (*it).first;
			if (pred(toDelete))
			{
				erase(toDelete);
			}
		}
	}


private:
    size_type m_size;
    std::unique_ptr<radix_tree_node<K, T, Compare>> m_root;

	Compare m_predicate;

    radix_tree_node<K, T, Compare>* begin(radix_tree_node<K, T, Compare> *node);
    radix_tree_node<K, T, Compare>* find_node(const K &key, radix_tree_node<K, T, Compare> *node, int depth);
    radix_tree_node<K, T, Compare>* append(radix_tree_node<K, T, Compare> *parent, const value_type &val);
    radix_tree_node<K, T, Compare>* prepend(radix_tree_node<K, T, Compare> *node, const value_type &val);
	void greedy_match(radix_tree_node<K, T, Compare> *node, std::vector<iterator> &vec);

    radix_tree(const radix_tree& other); // delete
    radix_tree& operator =(const radix_tree other); // delete
};

template <typename K, typename T, typename Compare>
void radix_tree<K, T, Compare>::prefix_match(const K &key, std::vector<iterator> &vec)
{
    vec.clear();

    if (m_root == NULL)
        return;

    radix_tree_node<K, T, Compare> *node;
    K key_sub1, key_sub2;

    node = find_node(key, m_root.get(), 0);

    if (node->m_is_leaf)
        node = node->m_parent;

    int len = radix_length(key) - node->m_depth;
    key_sub1 = radix_substr(key, node->m_depth, len);
    key_sub2 = radix_substr(node->m_key, 0, len);

    if (key_sub1 != key_sub2)
        return;

    greedy_match(node, vec);
}

template <typename K, typename T, typename Compare>
typename radix_tree<K, T, Compare>::iterator radix_tree<K, T, Compare>::longest_match(const K &key)
{
    if (m_root == NULL)
        return iterator(NULL);

    radix_tree_node<K, T, Compare> *node;
    K key_sub;

    node = find_node(key, m_root.get(), 0);

    if (node->m_is_leaf)
        return iterator(node);

    key_sub = radix_substr(key, node->m_depth, radix_length(node->m_key));

    if (! (key_sub == node->m_key))
        node = node->m_parent;

    K nul = radix_substr(key, 0, 0);

    while (node != NULL) {
        typename radix_tree_node<K, T, Compare>::it_child it;
        it = node->m_children.find(nul);
        if (it != node->m_children.end() && it->second.get()->m_is_leaf)
            return iterator(it->second.get());

        node = node->m_parent;
    }

    return iterator(NULL);
}


template <typename K, typename T, typename Compare>
typename radix_tree<K, T, Compare>::iterator radix_tree<K, T, Compare>::end()
{
    return iterator(NULL);
}

template <typename K, typename T, typename Compare>
typename radix_tree<K, T, Compare>::iterator radix_tree<K, T, Compare>::begin()
{
    radix_tree_node<K, T, Compare> *node;

    if (m_root == NULL || m_size == 0)
        node = NULL;
    else
        node = begin(m_root.get());

    return iterator(node);
}

template <typename K, typename T, typename Compare>
radix_tree_node<K, T, Compare>* radix_tree<K, T, Compare>::begin(radix_tree_node<K, T, Compare> *node)
{
    if (node->m_is_leaf)
        return node;


    assert(!node->m_children.empty());

    return begin(node->m_children.begin()->second.get());
}

template <typename K, typename T, typename Compare>
T& radix_tree<K, T, Compare>::operator[] (const K &lhs)
{
    iterator it = find(lhs);

    if (it == end()) {
        std::pair<K, T> val;
        val.first = lhs;

        std::pair<iterator, bool> ret;
        ret = insert(val);

        assert(ret.second == true);

        it = ret.first;
    }

    return it->second;
}

template <typename K, typename T, typename Compare>
void radix_tree<K, T, Compare>::greedy_match(const K &key, std::vector<iterator> &vec)
{
    radix_tree_node<K, T, Compare> *node;

    vec.clear();

    if (m_root == NULL)
        return;

    node = find_node(key, m_root.get(), 0);

    if (node->m_is_leaf)
        node = node->m_parent;

    greedy_match(node, vec);
}

template <typename K, typename T, typename Compare>
void radix_tree<K, T, Compare>::greedy_match(radix_tree_node<K, T, Compare> *node, std::vector<iterator> &vec)
{
    if (node->m_is_leaf) {
        vec.push_back(iterator(node));
        return;
    }

	typename std::map<K, radix_tree_node<K, T, Compare>*>::iterator it;

    for (it = node->m_children.begin(); it != node->m_children.end(); ++it) {
        greedy_match(it->second.get(), vec);
    }
}

template <typename K, typename T, typename Compare>
void radix_tree<K, T, Compare>::erase(iterator it)
{
    erase(it->first);
}

template <typename K, typename T, typename Compare>
bool radix_tree<K, T, Compare>::erase(const K &key)
{
	if (m_root == NULL)
		return 0;

	radix_tree_node<K, T, Compare> *child;
    radix_tree_node<K, T, Compare> *parent;
    radix_tree_node<K, T, Compare> *grandparent;
    K nul = radix_substr(key, 0, 0);

    child = find_node(key, m_root.get(), 0);

    if (! child->m_is_leaf)
        return 0;

    parent = child->m_parent;
    parent->m_children.erase(nul);

    m_size--;

    if (parent == m_root)
        return 1;

    if (parent->m_children.size() > 1)
        return 1;

    if (parent->m_children.empty()) {
        grandparent = parent->m_parent;
        grandparent->m_children.erase(parent->m_key);
    } else {
        grandparent = parent;
    }

    if (grandparent == m_root) {
        return 1;
    }

    if (grandparent->m_children.size() == 1) {
        // merge grandparent with the uncle
        typename std::map<K, radix_tree_node<K, T, Compare>*>::iterator it;
        it = grandparent->m_children.begin();

        auto uncle_ptr = it->second.get();

        if (uncle_ptr->m_is_leaf)
            return 1;

        // Transfer ownership of the uncle to a local variable.
        std::unique_ptr<radix_tree_node<K, T, Compare>> uncle = std::move(it->second);
        grandparent->m_children.erase(it);

        uncle->m_depth = grandparent->m_depth;
        uncle->m_key   = radix_join(grandparent->m_key, uncle->m_key);
        uncle->m_parent = grandparent->m_parent;

        // Transfer ownership of the uncle to the grandparent's parent's children.
        // Local uncle variable is no longer valid after this point.
        grandparent->m_parent->m_children[uncle->m_key] = std::move(uncle);
        
        // Erase the grandparent from the grandparent's parent's children.
        grandparent->m_parent->m_children.erase(grandparent->m_key);
    }

    return 1;
}


template <typename K, typename T, typename Compare>
radix_tree_node<K, T, Compare>* radix_tree<K, T, Compare>::append(radix_tree_node<K, T, Compare> *parent, const value_type &val)
{
    int depth;
    int len;
    K   nul = radix_substr(val.first, 0, 0);
    std::unique_ptr<radix_tree_node<K, T, Compare>> node_c, node_cc;

    depth = parent->m_depth + radix_length(parent->m_key);
    len   = radix_length(val.first) - depth;

    if (len == 0) {
        node_c = std::make_unique<radix_tree_node<K, T, Compare>>(val, m_predicate);
        
        // Grab the raw pointer to return.
        auto node_c_ptr = node_c.get();

        node_c->m_depth   = depth;
        node_c->m_parent  = parent;
        node_c->m_key     = nul;
        node_c->m_is_leaf = true;

        // Transfer ownership to parent's children map.
        // Local node_c variable is no longer valid after this point.
        parent->m_children[nul] = std::move(node_c);

        return node_c_ptr;
    } else {
        node_c = std::make_unique<radix_tree_node<K, T, Compare>>(val, m_predicate);
		node_cc = std::make_unique<radix_tree_node<K, T, Compare>>(val, m_predicate);

        // Grab the raw pointer to return.
        auto node_cc_ptr = node_cc.get();

        K key_sub = radix_substr(val.first, depth, len);

        node_c->m_depth  = depth;
        node_c->m_parent = parent;
        node_c->m_key    = key_sub;

        node_cc->m_depth   = depth + len;
        node_cc->m_parent  = node_c.get();
        node_cc->m_key     = nul;
        node_cc->m_is_leaf = true;

        // Transfer ownership to node_c's children map.
        // Local node_cc variable is no longer valid after this point.
        node_c->m_children[nul] = std::move(node_cc);

        // Transfer ownership to parent's children map.
        // Local node_c variable is no longer valid after this point.
        parent->m_children[key_sub] = std::move(node_c);

        return node_cc_ptr;
    }
}

template <typename K, typename T, typename Compare>
radix_tree_node<K, T, Compare>* radix_tree<K, T, Compare>::prepend(radix_tree_node<K, T, Compare> *node, const value_type &val)
{
    int count;
    int len1, len2;

    len1 = radix_length(node->m_key);
    len2 = radix_length(val.first) - node->m_depth;

    for (count = 0; count < len1 && count < len2; count++) {
        if (! (node->m_key[count] == val.first[count + node->m_depth]) )
            break;
    }

    assert(count != 0);

    // Transfer ownership to local_node variable.
    std::unique_ptr<radix_tree_node<K, T, Compare>> local_node = std::move(node->m_parent->m_children[node->m_key]);
    local_node->m_parent->m_children.erase(node->m_key);

    std::unique_ptr<radix_tree_node<K, T, Compare>> node_a = std::make_unique<radix_tree_node<K, T, Compare>>(m_predicate);
    auto node_a_ptr = node_a.get();

    node_a->m_parent = node->m_parent;
    node_a->m_key    = radix_substr(node->m_key, 0, count);
    node_a->m_depth  = node->m_depth;

    node->m_depth  += count;
    node->m_parent  = node_a_ptr;
    node->m_key     = radix_substr(node->m_key, count, len1 - count);
    
    // Transfer ownership of local_node to node's parent's children map.
    // Local node is no longer valid after this point.
    node->m_parent->m_children[node->m_key] = std::move(local_node);

    // Transfer ownership from local node_a variable to node_a's children map.
    // Local node_a is no longer valid after this point.
    node_a->m_parent->m_children[node_a->m_key] = std::move(node_a);

    K nul = radix_substr(val.first, 0, 0);
    if (count == len2) {
        std::unique_ptr<radix_tree_node<K, T, Compare>> node_b = std::make_unique<radix_tree_node<K, T, Compare>>(val, m_predicate);
        // Grab the raw pointer to return.
        auto node_b_ptr = node_b.get();

        node_b->m_parent  = node_a_ptr;
        node_b->m_key     = nul;
        node_b->m_depth   = node_a_ptr->m_depth + count;
        node_b->m_is_leaf = true;

        // Transfer ownership to parent's children map.
        node_b->m_parent->m_children[nul] = std::move(node_b);

        return node_b_ptr;
    } else {
        std::unique_ptr<radix_tree_node<K, T, Compare>> node_b, node_c;

        node_b = std::make_unique<radix_tree_node<K, T, Compare>>(m_predicate);
        auto node_b_ptr = node_b.get();

        node_c = std::make_unique<radix_tree_node<K, T, Compare>>(val, m_predicate);
        auto node_c_ptr = node_c.get();

        node_b->m_parent = node_a_ptr;
        node_b->m_depth  = node->m_depth;
        node_b->m_key    = radix_substr(val.first, node_b->m_depth, len2 - count);

        node_c->m_parent  = node_b_ptr;
        node_c->m_depth   = radix_length(val.first);
        node_c->m_key     = nul;
        node_c->m_is_leaf = true;
        
        // Transfer ownership to node_b's children map.
        node_c->m_parent->m_children[nul] = std::move(node_c);

        // Transfer ownership to parent's children map.
        node_b->m_parent->m_children[node_b->m_key] = std::move(node_b);

        return node_c_ptr;
    }
}

template <typename K, typename T, typename Compare>
std::pair<typename radix_tree<K, T, Compare>::iterator, bool> radix_tree<K, T, Compare>::insert(const value_type &val)
{
    if (m_root == NULL) {
        K nul = radix_substr(val.first, 0, 0);

        m_root = std::make_unique<radix_tree_node<K, T, Compare>>(m_predicate);
        m_root->m_key = nul;
    }

    radix_tree_node<K, T, Compare>* node = find_node(val.first, m_root.get(), 0);

    if (node->m_is_leaf) {
        return std::pair<iterator, bool>(node, false);
    } else if (node == m_root.get()) {
        m_size++;
        return std::pair<iterator, bool>(append(m_root.get(), val), true);
    } else {
        m_size++;
        int len     = radix_length(node->m_key);
        K   key_sub = radix_substr(val.first, node->m_depth, len);

        if (key_sub == node->m_key) {
            return std::pair<iterator, bool>(append(node, val), true);
        } else {
            return std::pair<iterator, bool>(prepend(node, val), true);
        }
    }
}

template <typename K, typename T, typename Compare>
typename radix_tree<K, T, Compare>::iterator radix_tree<K, T, Compare>::find(const K &key)
{
    if (m_root == NULL)
        return iterator(NULL);

    radix_tree_node<K, T, Compare> *node = find_node(key, m_root.get(), 0);

    // if the node is a internal node, return NULL
    if (! node->m_is_leaf)
        return iterator(NULL);

    return iterator(node);
}

template <typename K, typename T, typename Compare>
radix_tree_node<K, T, Compare>* radix_tree<K, T, Compare>::find_node(const K &key, radix_tree_node<K, T, Compare> *node, int depth)
{
    if (node->m_children.empty())
        return node;

    typename radix_tree_node<K, T, Compare>::it_child it;
    int len_key = radix_length(key) - depth;

    for (it = node->m_children.begin(); it != node->m_children.end(); ++it) {
        if (len_key == 0) {
            if (it->second->m_is_leaf)
                return it->second.get();
            else
                continue;
        }

        if (! it->second.get()->m_is_leaf && key[depth] == it->first[0] ) {
            int len_node = radix_length(it->first);
            K   key_sub = radix_substr(key, depth, len_node);

            if (key_sub == it->first) {
                return find_node(key, it->second.get(), depth + len_node);
            } else {
                return it->second.get();
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
