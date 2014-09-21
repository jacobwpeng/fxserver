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

#include <boost/scoped_ptr.hpp>
#include <boost/noncopyable.hpp>
#include <boost/property_tree/ptree_fwd.hpp>
#include "ring_buffer.h"

namespace fx
{
    namespace base
    {
        class ProcessBus : boost::noncopyable
        {
            public:
                typedef boost::function<int(const char*, int)> Validator;

            public:
                ProcessBus(unsigned bus_id, unsigned mmap_len, const std::string & filepath);
                ProcessBus(const boost::property_tree::ptree& pt);
                ~ProcessBus();

                int TryRecover();
                int Connect();
                int Listen();

                bool Write(const char * buf, int len);
                char * Read(int * plen);

                size_t size() const;
                static const size_t kMaxBufferBodyLength = RingBuffer::kMaxBufferBodyLength;

            private:
                struct Header
                {
                    bool locked;
                };

                bool Inited() const;
                char * InitHeader(char * start);
                int InitMMap(bool reuse);
                std::string mmap_filename() const;

            private:
                const unsigned bus_id_;
                void * mem_;
                Header * header_;
                const unsigned mmap_len_;
                int mmap_fd_;
                std::string filepath_;
                boost::scoped_ptr<fx::base::RingBuffer> buf_;
        };
    }
}

#endif   /* ----- #ifndef __PROCESS_BUS_H__  ----- */
