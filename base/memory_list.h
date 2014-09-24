/*
 * =====================================================================================
 *
 *       Filename:  memory_list.h
 *        Created:  08/22/14 21:43:12
 *         Author:  peng wang
 *          Email:  pw2191195@gmail.com
 *    Description:  
 *
 * =====================================================================================
 */

#ifndef  __MEMORY_LIST_H__
#define  __MEMORY_LIST_H__

#include <stdint.h>
#include <cstddef>

namespace fx
{
    namespace base
    {
        class MemoryList
        {
            public:
                typedef int32_t SliceId;
                static const SliceId kInvalidSliceId = -1;
                struct Slice
                {
                    SliceId next;
                    SliceId prev;
                    char buf[1];                /* WARNING: Use offsetof this field to get pointer size */
                };

            private:
                struct Header
                {
                    int64_t magic_number;
                    size_t padding;
                    size_t size;
                    size_t buffer_length;
                    size_t slice_size;
                    SliceId free;
                    SliceId used;
                };
                MemoryList(size_t slice_size);

            public:
                static const size_t kHeaderSize = sizeof(Header);
                ~MemoryList();

                static MemoryList* CreateFrom(void * start, size_t len, size_t slice_size);
                static MemoryList* RestoreFrom(void * start, size_t len, size_t slice_size);
                SliceId GetSlice();
                void FreeSlice(SliceId id);
                void * GetBuffer(SliceId id);
                size_t BufferLength() const { return header_->buffer_length; }
                size_t capacity() const { return capacity_; }
                size_t size() const { return header_->size; }
                void * start() const { return start_; }
                bool full() const { return header_->size == capacity_; }

            private:
                void MakeSliceList();
                Slice * GetSliceById(SliceId id);
                SliceId GetId(Slice * slice);

            private:
                static const int64_t kMagicNumber;
                static const size_t kSliceHeadSize;
                void * start_;
                void * end_;
                void * buf_;
                Header * header_;
                size_t capacity_;
                const size_t kSliceRealSize;
        };
    }
}
#endif   /* ----- #ifndef __MEMORY_LIST_H__  ----- */
