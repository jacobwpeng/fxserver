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
                ProcessBus(const boost::property_tree::ptree& pt);
                ~ProcessBus();

                int Connect();
                int Listen();

                bool Write(const char * buf, int len);
                char * Read(int * plen);

                size_t size() const;

            private:
                bool Inited() const;
                std::string mmap_filename() const;
                std::string fifo_filename() const;

            private:
                const unsigned bus_id_;
                void * mem_;
                const unsigned mmap_len_;
                int mmap_fd_;
                int fifo_fd_;
                std::string filepath_;
                boost::scoped_ptr<fx::base::RingBuffer> buf_;
        };
    }
}

#endif   /* ----- #ifndef __PROCESS_BUS_H__  ----- */
