/*
 * =====================================================================================
 *
 *       Filename:  list.h
 *        Created:  08/25/14 20:37:13
 *         Author:  peng wang
 *          Email:  pw2191195@gmail.com
 *    Description:  
 *
 * =====================================================================================
 */

#ifndef  __LIST_H__
#define  __LIST_H__

#include <memory>
#include <boost/scoped_ptr.hpp>
#include <boost/utility/enable_if.hpp>
#include <boost/type_traits/is_pod.hpp>
#include "memory_list.h"

namespace fx
{
    namespace base
    {
        namespace container
        {
            template <typename T, typename Enable = void>
            class List;

            template <typename T>
            class List<T, typename boost::enable_if< boost::is_pod<T> >::type >
            {
                public:
                    typedef fx::base::MemoryList::SliceId NodeId;
                private:
                    typedef List<T, typename boost::enable_if< boost::is_pod<T> >::type > Self;
                    static const size_t kMagicNumber = 8522338164747840960;
                private:
                    struct MetaData
                    {
                        int64_t magic_number;
                        size_t size;
                        NodeId head;
                        NodeId tail;
                    };

                    struct Node
                    {
                        NodeId prev;
                        NodeId next;
                        T val;
                    };

                    List()
                    {
                    }

                    Node * GetNode(NodeId id)
                    {
                        if (id == kInvalidNodeId) return NULL;
                        return reinterpret_cast<Node*>(ml_->GetBuffer(id));
                    }

                    const Node * GetNode(NodeId id) const
                    {
                        return const_cast<const Self*>(this)->GetNode(id);
                    }

                public:
                    static const NodeId kInvalidNodeId = fx::base::MemoryList::kInvalidSliceId;
                    static const size_t kHeaderSize = sizeof(MetaData);
                    static Self * CreateFrom(void * buf, size_t len)
                    {
                        if (len < kHeaderSize) return NULL;

                        std::unique_ptr<List> ptr(new List);
                        ptr->md_ = reinterpret_cast<MetaData*>(buf);
                        ptr->md_->magic_number = kMagicNumber;
                        ptr->md_->size = 0;
                        ptr->md_->head = kInvalidNodeId;
                        ptr->md_->tail = kInvalidNodeId;
                        uintptr_t start = reinterpret_cast<uintptr_t>(buf) + kHeaderSize;
                        ptr->ml_.reset (MemoryList::CreateFrom(reinterpret_cast<void*>(start), len - sizeof(MetaData), sizeof(Node)));
                        if (ptr->ml_ == NULL) return NULL;
                        return ptr.release();
                    }

                    static Self * RestoreFrom(void * buf, size_t len)
                    {
                        if (len < kHeaderSize) return NULL;

                        std::unique_ptr<List> ptr(new List);
                        MetaData * md = reinterpret_cast<MetaData*>(buf);
                        if (md->magic_number != kMagicNumber) return NULL;
                        ptr->md_ = md;

                        uintptr_t start = reinterpret_cast<uintptr_t>(buf) + kHeaderSize;
                        ptr->ml_.reset (MemoryList::RestoreFrom(reinterpret_cast<void*>(start), len - sizeof(MetaData), sizeof(Node)));
                        if (ptr->ml_ == NULL) return NULL;
                        return ptr.release();
                    }

                    T Unlink(NodeId id)
                    {
                        assert (id != kInvalidNodeId);
                        Node * node = GetNode(id);
                        Node * prev = GetNode(node->prev);
                        Node * next = GetNode(node->next);
                        if (prev)
                        {
                            prev->next = node->next;
                        }
                        else
                        {
                            md_->head = node->next;
                        }
                        if (next)
                        {
                            next->prev = node->prev;
                        }
                        else
                        {
                            md_->tail = node->prev;
                        }
                        --md_->size;
                        T val = node->val;
                        ml_->FreeSlice(id);
                        return val;
                    }

                    NodeId PushFront(T val)
                    {
                        if (ml_->full()) return kInvalidNodeId;

                        NodeId id = ml_->GetSlice();
                        Node * node = GetNode(id);
                        node->val = val;

                        if (md_->head == kInvalidNodeId)
                        {
                            assert (md_->tail == kInvalidNodeId);
                            md_->head = id;
                            md_->tail = id;
                            node->prev = kInvalidNodeId;
                            node->next = kInvalidNodeId;
                        }
                        else
                        {
                            Node * front = GetNode(md_->head);
                            assert (front);
                            assert (front->prev = kInvalidNodeId);
                            front->prev = id;
                            node->prev = kInvalidNodeId;
                            node->next = md_->head;
                            md_->head = id;
                        }
                        ++md_->size;
                        return id;
                    }

                    NodeId PushBack(T val)
                    {
                        if (ml_->full()) return kInvalidNodeId;

                        NodeId id = ml_->GetSlice();
                        Node * node = GetNode(id);
                        node->val = val;

                        if (md_->tail == kInvalidNodeId)
                        {
                            assert (md_->head == kInvalidNodeId);
                            md_->tail = id;
                            md_->head = id;
                            node->prev = kInvalidNodeId;
                            node->next = kInvalidNodeId;
                        }
                        else
                        {
                            Node * last = GetNode(md_->tail);
                            assert (last);
                            assert (last->next = kInvalidNodeId);
                            last->next = id;
                            node->prev = md_->tail;
                            node->next = kInvalidNodeId;
                            md_->tail = id;
                        }
                        ++md_->size;
                        return id;
                    }

                    T PopFront()
                    {
                        assert (md_->head != kInvalidNodeId);
                        assert (md_->tail != kInvalidNodeId);

                        return Unlink(md_->head);
                    }

                    T PopBack()
                    {
                        assert (md_->head != kInvalidNodeId);
                        assert (md_->tail != kInvalidNodeId);

                        return Unlink(md_->tail);
                    }

                    T Back()
                    {
                        assert (md_->tail != kInvalidNodeId);
                        Node * back = GetNode(md_->tail);
                        return back->val;
                    }

                    size_t size() const 
                    { 
                        assert (ml_->size() == md_->size);
                        return md_->size; 
                    }
                    size_t capacity() const { return ml_->capacity(); }
                    bool full() const { return size() == capacity(); }

                private:
                    MetaData * md_;
                    boost::scoped_ptr<fx::base::MemoryList> ml_;
            };
        }
    }
}

#endif   /* ----- #ifndef __LIST_H__  ----- */
