/*
 * =====================================================================================
 *
 *       Filename:  process_bus.cc
 *        Created:  08/10/14 22:57:40
 *         Author:  peng wang
 *          Email:  pw2191195@gmail.com
 *    Description:  
 *
 * =====================================================================================
 */

#include "process_bus.h"

#include <cstdio>
#include <fcntl.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <sys/mman.h>
#include <boost/property_tree/ptree.hpp>

fx::base::ProcessBus::ProcessBus(const boost::property_tree::ptree& pt)
    :bus_id_(pt.get<unsigned>("<xmlattr>.id"))
     , mmap_len_(pt.get<unsigned>("<xmlattr>.size")) 
     , mmap_fd_(-1)
     , fifo_fd_(-1)
     , filepath_(pt.get<std::string>("<xmlattr>.path"))
{
}

fx::base::ProcessBus::~ProcessBus()
{
    if (mem_)
    {
        ::munmap(mem_, mmap_len_);
    }

    if (mmap_fd_ != -1)
    {
        close(mmap_fd_);
    }

    if (fifo_fd_ != -1)
    {
        close(fifo_fd_);
    }
}

bool fx::base::ProcessBus::Inited() const
{
    return mmap_fd_ != -1;
}

int fx::base::ProcessBus::Connect()
{
    assert (not Inited());
    std::string mmap_name = mmap_filename();
    std::string fifo_name = fifo_filename();

    mmap_fd_ = open(mmap_name.c_str(), O_RDWR);
    if (mmap_fd_ < 0)
    {
        perror("open mmap");
        return -1;
    }

#ifndef NDEBUG
    struct stat sb;
    if (::fstat(mmap_fd_, &sb) < 0)
    {
        perror("fstat");
        return -2;
    }
    assert (sb.st_size == mmap_len_);
#endif

    mem_ = ::mmap(NULL, mmap_len_, PROT_READ | PROT_WRITE, MAP_SHARED, mmap_fd_, 0);
    if (mem_ == MAP_FAILED)
    {
        perror("mmap");
        return -3;
    }

    char * start = reinterpret_cast<char*>(mem_);
    char * end = start + mmap_len_;
    buf_.reset (new fx::base::RingBuffer(start, end, true));
    return 0;
}
