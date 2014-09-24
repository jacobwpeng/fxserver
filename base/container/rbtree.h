/*
 * =====================================================================================
 *
 *       Filename:  rbtree.h
 *        Created:  08/23/14 11:05:43
 *         Author:  peng wang
 *          Email:  pw2191195@gmail.com
 *    Description:  
 *
 * =====================================================================================
 */

#ifndef  __RBTREE_H__
#define  __RBTREE_H__

#include <cassert>
#include <memory>
#include <iostream>
#include <vector>
#include <boost/scoped_ptr.hpp>
#include <boost/utility/enable_if.hpp>
#include <boost/type_traits/is_pod.hpp>
#include <boost/type_traits/is_const.hpp>
#include "memory_list.h"

namespace fx
{
    namespace base
    {
        namespace container
        {
            template <typename _Key, typename _Value, typename Enable = void>
            class RBTree;

            template <typename _Key, typename _Value>
            class RBTree<_Key, _Value, typename boost::enable_if_c<boost::is_pod<_Key>::value and boost::is_pod<_Value>::value >::type>
            {
                public:
                    typedef RBTree<_Key, _Value, 
                            typename boost::enable_if_c<boost::is_pod<_Key>::value and boost::is_pod<_Value>::value >::type> ThisType;
                    typedef _Key KeyType;
                    typedef _Value ValueType;

                private:
                    typedef fx::base::MemoryList::SliceId NodeId;
                    static const NodeId kInvalidNodeId = fx::base::MemoryList::kInvalidSliceId;
                    static const int64_t kMagicNumber = 1050165255719967046;
                    struct MetaData
                    {
                        int64_t magic_number;
                        NodeId root;
                        size_t size;
                        size_t depth;
                    };

                    enum Color
                    {
                        kRed = 0,
                        kBlack = 1
                    };

                    struct RBNode
                    {
                        Color c;
                        NodeId p;               /* parent */
                        NodeId l;               /* left */
                        NodeId r;               /* right */
                        KeyType k;
                        ValueType v;

                        void Init(KeyType key, ValueType val)
                        {
                            c = kRed;
                            p = l = r = kInvalidNodeId;
                            k = key;
                            v = val;
                        }

                        void FlipColor()
                        {
                            c = (c == kRed ? kBlack : kRed);
                        }
                    };

                    template<typename ContainerType>
                    class ConstIterator;

                    template<typename ContainerType>
                    class Iterator : public std::iterator<std::bidirectional_iterator_tag, std::pair<_Key, _Value> >
                    {
                        typedef Iterator Self;
                        public:
                            Iterator(ContainerType * tree, NodeId id)
                                :tree_(tree), id_(id)
                            {
                            }

                            explicit Iterator(ContainerType * tree = NULL)
                                :tree_(tree), id_(kInvalidNodeId)
                            {
                            }

                            Iterator(const Self& rhs)
                            {
                                tree_ = rhs.tree_;
                                id_ = rhs.id_;
                            }

                            Self& operator ++ ()
                            {
                                assert (tree_);
                                if (id_ == kInvalidNodeId) return *this;
                                id_ = tree_->GetNext(id_);
                                return *this;
                            }

                            Self & operator -- ()
                            {
                                assert (tree_);
                                if (id_ == kInvalidNodeId)
                                {
                                    id_ = tree_->GetMax(tree_->md_->root);
                                }
                                else
                                {
                                    id_ = tree_->GetPrev(id_);
                                }
                                return *this;
                            }

                            Self& operator = (const Self& rhs)
                            {
                                tree_ = rhs.tree_;
                                id_ = rhs.id_;
                                return *this;
                            }

                            bool operator == (const Self& rhs) const
                            {
                                return tree_ == rhs.tree_ and id_ == rhs.id_;
                            }

                            bool operator != (const Self& rhs) const
                            {
                                return not (*this == rhs);
                            }

                            std::pair<KeyType, ValueType&> operator * (){ return std::make_pair(Key(), Value()); }

                            KeyType Key() const { return tree_->GetPointer(id_)->k; }
                            ValueType Value() const { return tree_->GetPointer(id_)->v; }
                            ValueType & Value() { return tree_->GetPointer(id_)->v; }

                        private:
                            friend class ConstIterator <ContainerType>;
                            ContainerType * tree_;
                            NodeId id_;
                    };

                    template<typename ContainerType>
                    class ConstIterator
                    {
                        typedef ConstIterator Self;
                        public:
                            ConstIterator(const ContainerType * tree, NodeId id)
                                :tree_(tree), id_(id)
                            {
                            }

                            explicit ConstIterator(const ContainerType * tree = NULL)
                                :tree_(tree), id_(kInvalidNodeId)
                            {
                            }

                            ConstIterator(const Self& rhs)
                            {
                                tree_ = rhs.tree_;
                                id_ = rhs.id_;
                            }

                            Self& operator ++ ()
                            {
                                assert (id_ != kInvalidNodeId);
                                id_ = tree_->GetNext(id_);
                                return *this;
                            }

                            Self& operator = (const Self& rhs)
                            {
                                tree_ = rhs.tree_;
                                id_ = rhs.id_;
                                return *this;
                            }

                            Self& operator = (const Iterator<ContainerType>& rhs)
                            {
                                tree_ = rhs.tree_;
                                id_ = rhs.id_;
                                return *this;
                            }

                            bool operator == (const Self& rhs) const
                            {
                                return tree_ == rhs.tree_ and id_ == rhs.id_;
                            }

                            bool operator != (const Self& rhs) const
                            {
                                return not (*this == rhs);
                            }

                            KeyType Key() const { return tree_->GetPointer(id_)->k; }
                            ValueType Value() const { return tree_->GetPointer(id_)->v; }
                        private:
                            const ContainerType * tree_;
                            NodeId id_;
                    };

                    RBTree()
                    {
                    }

                public:
                    typedef Iterator<ThisType> iterator;
                    typedef ConstIterator<ThisType> const_iterator;
                    iterator begin() 
                    { 
                        if (md_->root == kInvalidNodeId) return iterator(this);
                        else return iterator(this, GetMin(md_->root));
                    }
                    iterator end() { return iterator(this); }

                    const_iterator begin() const
                    {
                        if (md_->root == kInvalidNodeId) return const_iterator(this);
                        else return const_iterator(this, GetMin(md_->root));
                    }

                    const_iterator end() const { return const_iterator(this); }

                private:
                    RBNode * GetPointer(NodeId id)
                    {
                        if (id == kInvalidNodeId) return NULL;
                        return reinterpret_cast<RBNode*>( ml_->GetBuffer(id) );
                    }

                    const RBNode * GetPointer(NodeId id) const
                    {
                        return const_cast<ThisType*>(this)->GetPointer(id);
                    }

                    NodeId KeepBalance(NodeId node_id)
                    {
                        RBNode * node = GetPointer(node_id);
                        assert (node);

                        RBNode * right = GetPointer(node->r);
                        if (right and right->c == kRed)
                        {
                            node_id = RotateLeft(node_id);
                            node = GetPointer(node_id);
                            assert (node);
                        }


                        RBNode * left = GetPointer(node->l);
                        if (left and left->c == kRed)
                        {
                            RBNode * lol = GetPointer(left->l);
                            if (lol and lol->c == kRed)
                            {
                                node_id = RotateRight(node_id);
                                node = GetPointer(node_id);
                                assert (node);
                            }
                        }

                        right = GetPointer(node->r);
                        left = GetPointer(node->l);

                        if (left and left->c == kRed and right and right->c == kRed)
                        {
                            FlipColors(node_id);
                        }

                        return node_id;
                    }

                    NodeId RotateLeft(NodeId id)
                    {
                        RBNode * node = GetPointer(id);
                        assert (node);
                        NodeId right_id = node->r;
                        RBNode * right = GetPointer(right_id);
                        assert (right);
                        assert (right->c == kRed);
                        node->r = right->l;
                        right->l = id;
                        right->p = node->p;
                        node->p = right_id;

                        right->c = node->c;
                        node->c = kRed;

                        return right_id;
                    }

                    NodeId RotateRight(NodeId id)
                    {
                        RBNode * node = GetPointer(id);
                        assert (node);

                        NodeId left_id = node->l;
                        RBNode * left = GetPointer(left_id);
                        assert (left);
                        assert (left->c == kRed);

                        node->l = left->r;
                        left->r = id;
                        left->p = node->p;
                        node->p = left_id;

                        left->c = node->c;
                        node->c = kRed;

                        return left_id;
                    }

                    void FlipColors(NodeId id)
                    {
                        RBNode * node = GetPointer(id);
                        assert (node);
                        RBNode * left = GetPointer(node->l);
                        assert (left);
                        RBNode * right = GetPointer(node->r);
                        assert (right);

                        node->FlipColor();
                        left->FlipColor();
                        right->FlipColor();
                    }

                    NodeId Put(NodeId node_id, KeyType key, ValueType val)
                    {
                        RBNode * node = GetPointer(node_id);
                        if (node == NULL)
                        {
                            if (ml_->size() == ml_->capacity()) throw 0;
                            NodeId new_id = ml_->GetSlice();
                            RBNode * new_node = GetPointer(new_id);
                            new_node->Init(key, val);
                            ++md_->size;
                            return new_id;
                        }
                        else
                        {
                            if (key > node->k)
                            {
                                node->r = Put(node->r, key, val);
                                node_id = KeepBalance(node_id);
                            }
                            else if (key < node->k)
                            {
                                node->l = Put(node->l, key, val);
                                node_id = KeepBalance(node_id);
                            }
                            else
                            {
                                node->v = val;
                            }
                            return node_id;
                        }
                    }

                    NodeId Get(NodeId id, KeyType key) const
                    {
                        const RBNode * node = GetPointer(id);
                        if (not node) return kInvalidNodeId;
                        else if (node->k < key)
                        {
                            return Get(node->r, key);
                        }
                        else if (node->k > key)
                        {
                            return Get(node->l, key);
                        }
                        else
                        {
                            return id;
                        }
                    }

                    NodeId GetMin(NodeId node_id) const
                    {
                        const RBNode * node = GetPointer(node_id);
                        assert (node);
                        while (node->l != kInvalidNodeId)
                        {
                            node_id = node->l;
                            node = GetPointer(node_id);
                        }
                        return node_id;
                    }

                    NodeId GetMax(NodeId node_id) const
                    {
                        const RBNode * node = GetPointer(node_id);
                        assert (node);
                        while (node->r != kInvalidNodeId)
                        {
                            node_id = node->r;
                            node = GetPointer(node_id);
                        }
                        return node_id;
                    }

                    NodeId GetPrev(NodeId node_id) const
                    {
                        const RBNode * node = GetPointer(node_id);
                        if (node == NULL) return kInvalidNodeId;
                        else if (node->l != kInvalidNodeId)
                        {
                            return GetMax(node->l);
                        }
                        else
                        {
                            NodeId cur = node_id;
                            NodeId parent = node->p;
                            const RBNode * p = GetPointer(parent);
                            while (p and p->l == cur)
                            {
                                cur = parent;
                                parent = p->p;
                                p = GetPointer(parent);
                            }
                            if (p) return parent;
                            else return kInvalidNodeId;
                        }
                    }
                    
                    NodeId GetNext(NodeId node_id) const
                    {
                        const RBNode * node = GetPointer(node_id);
                        if (node == NULL) return kInvalidNodeId;
                        else if (node->r != kInvalidNodeId)
                        {
                            return GetMin(node->r);
                        }
                        else
                        {
                            NodeId cur = node_id;
                            NodeId parent = node->p;
                            const RBNode * p = GetPointer(parent);
                            while (p and p->r == cur)
                            {
                                cur = parent;
                                parent = p->p;
                                p = GetPointer(parent);
                            }
                            if (p) return parent;
                            else return kInvalidNodeId;
                        }
                    }

                    NodeId DeleteMin(NodeId node_id)
                    {
                        RBNode * node = GetPointer(node_id);
                        assert (node);

                        if (node->l == kInvalidNodeId)
                        {
                            ml_->FreeSlice(node_id);
                            --md_->size;
                            return kInvalidNodeId;
                        }
                        RBNode * left = GetPointer(node->l);
                        assert (left);

                        if (left->c == kBlack)
                        {
                            RBNode * lol = GetPointer(left->l);
                            if (lol == NULL or lol->c == kBlack)
                            {
                                node_id = MoveRedLeft(node_id);
                            }
                        }

                        node = GetPointer(node_id);
                        node->l = DeleteMin(node->l);
                        return KeepBalance(node_id);
                    }

                    NodeId MoveRedLeft(NodeId node_id)
                    {
                        RBNode * node = GetPointer(node_id);
                        assert (node);
                        assert (node->c == kRed);

                        RBNode * left = GetPointer(node->l);
                        assert (left);

                        assert (left->c == kBlack);
                        RBNode * lol = GetPointer(left->l);
                        assert (lol == NULL or lol->c == kBlack);

                        FlipColors(node_id);

                        RBNode * right = GetPointer(node->r);
                        assert (right);
                        RBNode * lor = GetPointer(right->l);
                        if (lor and lor->c == kRed)
                        {
                            node->r = RotateRight(node->r);
                            node_id = RotateLeft(node_id);
                            FlipColors(node_id);
                        }

                        return node_id;
                    }

                    NodeId MoveRedRight(NodeId node_id)
                    {
                        RBNode * node = GetPointer(node_id);
                        assert (node);

                        RBNode * left = GetPointer(node->l);
                        assert (left);

                        RBNode * right = GetPointer(node->r);
                        assert (right);

                        assert (right->c == kBlack);
                        RBNode * ror = GetPointer(right->r);
                        assert (ror == NULL or ror->c == kBlack);

                        FlipColors(node_id);
                        RBNode * lol = GetPointer(left->l);
                        if (lol and lol->c == kRed)
                        {
                            node_id = RotateRight(node_id);
                            FlipColors(node_id);
                        }
                        return node_id;
                    }

                    NodeId Delete(NodeId node_id, KeyType key)
                    {
                        RBNode * node = GetPointer(node_id);
                        assert (node);

                        RBNode * left = GetPointer(node->l);
                        if (key < node->k)
                        {
                            if (left == NULL) throw 0;
                            if (left->c == kBlack)
                            {
                                RBNode * lol = GetPointer(left->l);
                                if (lol == NULL or lol->c == kBlack)
                                {
                                    node_id = MoveRedLeft(node_id);
                                    node = GetPointer(node_id);
                                }
                            }
                            node->l = Delete(node->l, key);
                        }
                        else
                        {
                            if (left != NULL and left->c == kRed)
                            {
                                //borrow from sibling
                                node_id = RotateRight(node_id);
                                node = GetPointer(node_id);
                            }

                            if (key == node->k and node->r == kInvalidNodeId)
                            {
                                ml_->FreeSlice(node_id);
                                --md_->size;
                                return kInvalidNodeId;
                            }

                            RBNode * right = GetPointer(node->r);
                            if (right and right->c == kBlack)
                            {
                                RBNode * lor = GetPointer(right->l);
                                if (lor == NULL or lor->c == kBlack)
                                {
                                    node_id = MoveRedRight(node_id);
                                    node = GetPointer(node_id);
                                }
                            }

                            if (key == node->k)
                            {
                                NodeId successor_id = GetMin(node->r);
                                RBNode * successor = GetPointer(successor_id);
                                assert (successor);
                                node->k = successor->k;
                                node->v = successor->v;
                                node->r = DeleteMin(node->r);
                            }
                            else
                            {
                                if (node->r == kInvalidNodeId) throw 0;
                                node->r = Delete(node->r, key);
                            }
                        }
                        return KeepBalance(node_id);
                    }

                public:
                    static RBTree * CreateFrom(void * buf, size_t len)
                    {
                        if (len < sizeof(MetaData)) return NULL;
                        std::unique_ptr<RBTree> ptr(new RBTree);
                        ptr->md_ = reinterpret_cast<MetaData*>(buf);
                        ptr->md_->magic_number = kMagicNumber;
                        ptr->md_->root = kInvalidNodeId;
                        ptr->md_->size = 0;
                        ptr->md_->depth = 0;
                        uintptr_t start = reinterpret_cast<uintptr_t>(buf) + sizeof(MetaData);
                        ptr->ml_.reset(fx::base::MemoryList::CreateFrom(reinterpret_cast<void*>(start), len - sizeof(MetaData), sizeof(RBNode)));
                        if (ptr->ml_ == NULL) return NULL;
                        return ptr.release();
                    }

                    static RBTree * RestoreFrom(void * buf, size_t len)
                    {
                        if (len < sizeof(MetaData)) return NULL;
                        MetaData * md = reinterpret_cast<MetaData*>(buf);
                        if (md->magic_number != kMagicNumber) return NULL;

                        std::unique_ptr<RBTree> ptr(new RBTree);
                        ptr->md_ = md;
                        uintptr_t start = reinterpret_cast<uintptr_t>(buf) + sizeof(MetaData);
                        ptr->ml_.reset(fx::base::MemoryList::CreateFrom(reinterpret_cast<void*>(start), len - sizeof(MetaData), sizeof(RBNode)));
                        if (ptr->ml_ == NULL) return NULL;
                        return ptr.release();
                    }

                    size_t size() const 
                    {
                        return md_->size;
                    }

                    size_t depth() const
                    {
                        return md_->depth;
                    }

                    size_t capacity() const
                    {
                        return ml_->capacity();
                    }

                    bool Put(KeyType key, ValueType val)
                    {
                        try
                        {
                            md_->root = Put(md_->root, key, val);
                        }
                        catch(int)
                        {
                            return false;
                        }
                        RBNode * node = GetPointer(md_->root);
                        assert (node);
                        if (node->c == kRed)
                        {
                            node->c = kBlack;
                            ++md_->depth;
                        }
                        return true;
                    }

                    size_t Delete(KeyType key)
                    {
                        if (md_->root == kInvalidNodeId) return 0;
                        RBNode * root = GetPointer(md_->root);
                        root->c = kRed;
                        try
                        {
                            md_->root = Delete(md_->root, key);
                            if (md_->root != kInvalidNodeId) GetPointer(md_->root)->c = kBlack;
                            return 1;
                        }
                        catch (int)
                        {
                            if (md_->root != kInvalidNodeId) GetPointer(md_->root)->c = kBlack;
                            return 0;
                        }
                    }

                    void Clear()
                    {
                        ml_.reset (fx::base::MemoryList::CreateFrom(ml_->start(), ml_->BufferLength(), sizeof(RBNode)));
                        md_->size = 0;
                        md_->depth = 0;
                        md_->root = kInvalidNodeId;
                    }

                    iterator Get(KeyType key)
                    {
                        return iterator(this, Get(md_->root, key));
                    }

                    const_iterator Get(KeyType key) const
                    {
                        return const_iterator(this, Get(md_->root, key));
                    }
#ifndef NDEBUG
                    void LevelOrderTraversal()
                    {
                        if (md_->root == kInvalidNodeId) return;
                        std::vector<NodeId> nodes;
                        nodes.push_back(md_->root);

                        size_t cur = 0, last = 1;

                        while (cur < nodes.size())
                        {
                            last = nodes.size();

                            while (cur < last)
                            {
                                NodeId now = nodes[cur];
                                RBNode * node = GetPointer(now);
                                assert (node);

                                std::cout << node->k << "(" << (node->c == kRed ? "red" : "black") << ")\t";
                                if (node->l != kInvalidNodeId) nodes.push_back(node->l);
                                if (node->r != kInvalidNodeId) nodes.push_back(node->r);

                                ++cur;
                            }
                            std::cout << '\n';
                        }
                        std::cout << "********************************************************************************\n";
                    }
#endif

                private:
                    MetaData * md_;
                    boost::scoped_ptr<fx::base::MemoryList> ml_;
            };
        }
    }
}

#endif   /* ----- #ifndef __RBTREE_H__  ----- */
