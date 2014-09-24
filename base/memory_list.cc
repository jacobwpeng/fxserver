/*
 * =====================================================================================
 *
 *       Filename:  memory_list.cc
 *        Created:  08/22/14 21:48:44
 *         Author:  peng wang
 *          Email:  pw2191195@gmail.com
 *    Description:  
 *
 * =====================================================================================
 */

#include "memory_list.h"
#include <cassert>
#include <iostream>
#include <memory>

namespace fx
{
    namespace base
    {
        const int64_t MemoryList::kMagicNumber = -356060050714630365;
        const size_t MemoryList::kSliceHeadSize = sizeof(MemoryList::SliceId) * 2;

        MemoryList::MemoryList(size_t slice_size)
            :kSliceRealSize(kSliceHeadSize + slice_size)
        {
        }

        MemoryList::~MemoryList()
        {
        }

        MemoryList* MemoryList::CreateFrom(void * start, size_t len, size_t slice_size)
        {
            std::unique_ptr<MemoryList> ptr(new MemoryList(slice_size));
            uintptr_t s = reinterpret_cast<uintptr_t>(start);
            void * end = reinterpret_cast<void*>(s + len);
            uintptr_t e = reinterpret_cast<uintptr_t>(end);

            assert (end > start);
            if (s + sizeof(Header) > e) return NULL;

            ptr->header_ = reinterpret_cast<Header*>(start);
            ptr->header_->buffer_length = len;
            ptr->header_->magic_number = MemoryList::kMagicNumber;
            ptr->header_->slice_size = slice_size;
            ptr->header_->used = kInvalidSliceId;
            ptr->header_->size = 0;
            ptr->start_ = start;
            ptr->end_ = end;
            ptr->buf_ = reinterpret_cast<void*>( s+sizeof(Header) );
            ptr->MakeSliceList();
            return ptr.release();
        }

        MemoryList * MemoryList::RestoreFrom(void * start, size_t len, size_t slice_size)
        {
            std::unique_ptr<MemoryList> ptr(new MemoryList(slice_size));
            uintptr_t s = reinterpret_cast<uintptr_t>(start);
            void * end = reinterpret_cast<void*>(s + len);
            uintptr_t e = reinterpret_cast<uintptr_t>(end);
            //WARNING : Maybe overflow
            if (s + sizeof(Header) >= e) { return NULL; }

            Header * header = reinterpret_cast<Header*>(s);
            if (header->buffer_length > len   /* allow growing up */
                    || header->magic_number != MemoryList::kMagicNumber
                    || header->slice_size != slice_size)
            {
                return NULL;
            }
            ptr->header_ = header;
            ptr->start_ = start;
            ptr->end_ = end;
            ptr->buf_ = reinterpret_cast<void*>( s+sizeof(Header) );
            ptr->capacity_ = (len - ptr->header_->padding - kHeaderSize) / (kSliceHeadSize + slice_size);

            return ptr.release();
        }

        MemoryList::SliceId MemoryList::GetSlice()
        {
            if (header_->free == kInvalidSliceId) { return kInvalidSliceId; }
            else
            {
                SliceId slice_id = header_->free;
                Slice * slice = GetSliceById(slice_id);
                header_->free = slice->next;

                Slice * next = GetSliceById(slice->next);
                if (next) { next->prev = kInvalidSliceId; }

                SliceId old_head = header_->used;
                /* put slice in the front */
                header_->used = slice_id;
                slice->prev = kInvalidSliceId;   
                slice->next = old_head;
                Slice * old = GetSliceById(old_head);
                if (old) { old->prev = slice_id; }
                ++header_->size;
                return slice_id;
            }
        }

        void MemoryList::FreeSlice(MemoryList::SliceId id)
        {
            if (id == kInvalidSliceId) return;

            assert (id >= 0);
            assert (id < static_cast<SliceId>(capacity_));

            Slice * slice = GetSliceById(id);
            //unlink self from used list
            Slice * prev = GetSliceById(slice->prev);
            Slice * next = GetSliceById(slice->next);
            if (prev)
            {
                prev->next = slice->next;
            }
            if (next)
            {
                next->prev = slice->prev;
            }
            if (header_->used == id)
            {
                assert (prev == NULL);
                header_->used = slice->next;
            }
            //put slice in the front of free list
            SliceId old_head = header_->free;
            slice->prev = kInvalidSliceId;
            slice->next = header_->free;
            header_->free = id;
            Slice * old = GetSliceById(old_head);
            if (old) old->prev = id;

            --header_->size;
        }

        void * MemoryList::GetBuffer(SliceId id)
        {
            Slice * slice = GetSliceById(id);
            assert (slice);
            return slice->buf;
        }

        void MemoryList::MakeSliceList()
        {
            uintptr_t e = reinterpret_cast<uintptr_t>(end_);
            uintptr_t cur = reinterpret_cast<uintptr_t>(buf_);

            SliceId prev = kInvalidSliceId;
            while (cur + kSliceRealSize <= e)
            {
                Slice * slice = reinterpret_cast<Slice*>(cur);
                slice->prev = prev;
                slice->next = prev + 2;
                ++prev;
                cur += kSliceRealSize;

                if (cur + kSliceRealSize > e)
                {
                    //last one
                    slice->next = kInvalidSliceId;
                }
            }
            header_->padding = e - cur;
            if (prev == kInvalidSliceId)
            {
                header_->free = kInvalidSliceId;
                capacity_ = 0;
            }
            else
            {
                header_->free = 0;
                capacity_ = prev + 1;
            }
        }

        MemoryList::Slice * MemoryList::GetSliceById(SliceId id)
        {
            if (id == kInvalidSliceId) { return NULL; }
            assert (id >= 0);
            return reinterpret_cast<Slice*>( reinterpret_cast<uintptr_t>(buf_) + kSliceRealSize * id);
        }

        MemoryList::SliceId MemoryList::GetId(Slice * slice)
        {
            if (slice == NULL) { return kInvalidSliceId; }
            ptrdiff_t diff = reinterpret_cast<uintptr_t>(slice) - reinterpret_cast<uintptr_t>(buf_);
#ifndef NDEBUG
            assert (diff % kSliceRealSize == 0);
#endif
            return diff / kSliceRealSize;
        }
    }
}
