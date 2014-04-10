/*
 * =====================================================================================
 *       Filename:  fx_buffer.h
 *        Created:  11:17:22 Apr 10, 2014
 *         Author:  jacobwpeng
 *          Email:  jacobwpeng@tencent.com
 *    Description:  
 *
 * =====================================================================================
 */

#ifndef  __FX_BUFFER_H__
#define  __FX_BUFFER_H__

#include <string>
#include <vector>

namespace fx
{
    class Buffer
    {
        public:
            Buffer(size_t reserved_size = kDefaultReservedSize);
            ~Buffer();

            /* 不触发内部容器大小调整的写 */
            size_t BytesCanWrite() const;
            char * WriteBegin();                /* 返回连续内存区域 */
            void AddBytes(size_t len);            /* 写入成功时候增加的长度 */

            /* 从Buf读取数据 */
            size_t BytesToRead() const;
            char * ReadBegin();
            /* 读取成功消耗掉的长度 */
            void ConsumeBytes(size_t len);

            /* 保证写成功，可能触发容器大小调整 */
            void Append( const char * buf, size_t len );

            std::string ReadAndClear();

            void Clear();

            size_t ByteSize() const;

        private:
            void EnsureSpace(size_t len);

        private:
            static const size_t kDefaultReservedSize = 1 << 16; /* 默认给你64K */
            std::vector<char> internal_buf_;
            size_t read_index_;
            size_t write_index_;
    };
}

#endif   /* ----- #ifndef __FX_BUFFER_H__----- */
