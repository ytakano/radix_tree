#ifndef RADIX_TREE_IT
#define RADIX_TREE_IT

#include <iterator>

// forward declaration
template <typename K, typename T> class radix_tree;
template <typename K, typename T> class radix_tree_node;

template <typename K, typename T>
class radix_tree_it : public std::iterator<std::bidirectional_iterator_tag,
                                           std::pair<K, T> > {
        friend class radix_tree<K, T>;

public:
        radix_tree_it() { }
        ~radix_tree_it() { }

        std::pair<const K, T>&     operator*  () const;
        std::pair<const K, T>*     operator-> () const;
        const radix_tree_it<K, T>& operator++ ();
        const radix_tree_it<K, T>& operator-- ();
        bool        operator!= (const radix_tree_it<K, T> &lhs) const;
        bool        operator== (const radix_tree_it<K, T> &lhs) const;

private:
        radix_tree_node<K, T> *m_pointee;
        radix_tree_it(radix_tree_node<K, T> *p) : m_pointee(p) { }
};

template <typename K, typename T>
std::pair<const K, T>&
radix_tree_it<K, T>::operator* () const
{
        return m_pointee->m_value;
}

template <typename K, typename T>
std::pair<const K, T>*
radix_tree_it<K, T>::operator-> () const
{
        return &m_pointee->m_value;
}

template <typename K, typename T>
bool
radix_tree_it<K, T>::operator!= (const radix_tree_it<K, T> &lhs) const
{
        return m_pointee != lhs.m_pointee;
}

template <typename K, typename T>
bool
radix_tree_it<K, T>::operator== (const radix_tree_it<K, T> &lhs) const
{
        return m_pointee == lhs.m_pointee;
}

template <typename K, typename T>
const radix_tree_it<K, T>&
radix_tree_it<K, T>::operator++ ()
{
        assert(m_pointee != NULL);

        return *this;
}

template <typename K, typename T>
const radix_tree_it<K, T>&
radix_tree_it<K, T>::operator-- ()
{
        assert(m_pointee != NULL);

        return *this;
}

#endif RADIX_TREE_IT
