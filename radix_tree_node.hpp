#ifndef RADIX_TREE_NODE_HPP
#define RADIX_TREE_NODE_HPP

#include <functional>
#include <map>
#include <memory>

template <typename K, typename T, typename Compare>
class radix_tree_node {
    friend class radix_tree<K, T, Compare>;
    friend class radix_tree_it<K, T, Compare>;

    typedef std::pair<const K, T> value_type;
    typedef typename std::map<K, std::unique_ptr<radix_tree_node<K, T, Compare>>, Compare >::iterator it_child;

public:
	radix_tree_node(Compare& pred) : m_children(std::map<K, std::unique_ptr<radix_tree_node<K, T, Compare>>, Compare>(pred)), m_parent(NULL), m_depth(0), m_is_leaf(false), m_key(), m_pred(pred) { }
    radix_tree_node(const value_type &val, Compare& pred);
    radix_tree_node(const radix_tree_node&); // delete
    radix_tree_node& operator=(const radix_tree_node&); // delete

    ~radix_tree_node();

  private:
    std::unique_ptr<radix_tree_node<K, T, Compare>>&& remove_child(const K &key);

    // This class owns instances of radix_tree_node stored in m_children.
    std::map<K, std::unique_ptr<radix_tree_node<K, T, Compare>>, Compare> m_children;
    
    // This class does not own instances of radix_tree_node stored in m_parent.
    radix_tree_node<K, T, Compare> *m_parent;
    
    // This class owns instances of value_type stored in m_value.
    std::unique_ptr<value_type> m_value;
    int m_depth;
    bool m_is_leaf;
    K m_key;
	Compare& m_pred;
};

template <typename K, typename T, typename Compare>
radix_tree_node<K, T, Compare>::radix_tree_node(const value_type &val, Compare& pred) :
    m_children(std::map<K, std::unique_ptr<radix_tree_node<K, T, Compare>>, Compare>(pred)),
    m_parent(NULL),
    m_depth(0),
    m_is_leaf(false),
    m_key(), 
	m_pred(pred)
{
    m_value = std::make_unique<value_type>(val);
}

template <typename K, typename T, typename Compare>
radix_tree_node<K, T, Compare>::~radix_tree_node()
{
    m_value.reset();
}

#endif // RADIX_TREE_NODE_HPP
