/*
 * =====================================================================================
 *
 *       Filename:  process_bus.h
 *        Created:  08/10/14 22:23:07
 *         Author:  peng wang
 *          Email:  pw2191195@gmail.com
 *    Description:  
 *
 * =====================================================================================
 */

#ifndef  __PROCESS_BUS_H__
#define  __PROCESS_BUS_H__

#include <memory>
#include "container/ring_buffer.h"
#include "mmap_file.h"

namespace fx
{
    namespace base
    {
        namespace bus
        {
            class ProcessBus
            {
                private:
                    struct Header
                    {
                        const static int64_t kMagicNumber = 7316964413220353303;
                        int64_t magic_number;
                    };
                    const static size_t kHeaderSize = sizeof(Header);

                private:
                    ProcessBus();

                public:
                    static std::unique_ptr<ProcessBus> RestoreFrom(const std::string& filepath, size_t size);
                    static std::unique_ptr<ProcessBus> CreateFrom(const std::string& filepath, size_t size);
                    static std::unique_ptr<ProcessBus> ConnectTo(const std::string & filepath, size_t size);
                    static const size_t kMaxBufferBodyLength = container::RingBuffer::kMaxBufferBodyLength;

                    bool Write(const char * buf, int len);
                    char * Read(int * plen);

                    size_t size() const;
                private:
                    Header * header_;
                    std::unique_ptr<fx::base::container::RingBuffer> buf_;
                    std::unique_ptr<fx::base::MMapFile> file_;
            };
        }
    }
}

#endif   /* ----- #ifndef __PROCESS_BUS_H__  ----- */
