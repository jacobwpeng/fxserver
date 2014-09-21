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
#include <vector>
#include <fcntl.h>
#include <unistd.h>
#include <errno.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <sys/mman.h>
#include <boost/property_tree/ptree.hpp>

fx::base::ProcessBus::ProcessBus(unsigned bus_id, unsigned mmap_len, const std::string & filepath)
    :bus_id_(bus_id)
     , mem_(NULL)
     , header_(NULL)
     , mmap_len_(mmap_len) 
     , mmap_fd_(-1)
     , filepath_(filepath)
{
}

fx::base::ProcessBus::ProcessBus(const boost::property_tree::ptree& pt)
    :bus_id_(pt.get<unsigned>("<xmlattr>.id"))
     , mem_(NULL)
     , header_(NULL)
     , mmap_len_(pt.get<unsigned>("<xmlattr>.size")) 
     , mmap_fd_(-1)
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
}

bool fx::base::ProcessBus::Inited() const
{
    return mmap_fd_ != -1;
}

bool fx::base::ProcessBus::Write(const char * buf, int len)
{
    return buf_->Push(buf, len);
}

char * fx::base::ProcessBus::Read(int * plen)
{
    return buf_->Pop(plen);
}

size_t fx::base::ProcessBus::size() const
{
    return buf_->element_size();
}

char * fx::base::ProcessBus::InitHeader(char * start)
{
    header_ = reinterpret_cast<Header*>(start);
    start += sizeof(Header);
    return start;
}

int fx::base::ProcessBus::InitMMap(bool reuse)
{
    std::string mmap_name = mmap_filename();

    mmap_fd_ = open(mmap_name.c_str(), O_RDWR | O_CREAT, 0666);
    if (mmap_fd_ < 0)
    {
        perror("open mmap");
        return -1;
    }
    ::ftruncate(mmap_fd_, mmap_len_);

    mem_ = ::mmap(NULL, mmap_len_, PROT_READ | PROT_WRITE, MAP_SHARED, mmap_fd_, 0);
    if (mem_ == MAP_FAILED)
    {
        perror("mmap");
        return -2;
    }

    char * start = reinterpret_cast<char*>(mem_);
    char * end = start + mmap_len_;
    buf_.reset (new fx::base::RingBuffer(start, end, reuse));
    return 0;
}

int fx::base::ProcessBus::TryRecover()
{
    assert (not Inited());
    std::string mmap_name = mmap_filename();

    mmap_fd_ = open(mmap_name.c_str(), O_RDWR | O_CREAT, 0666);
    if (mmap_fd_ < 0)
    {
        perror("open mmap");
        return -1;
    }
    struct stat sb;
    if (fstat(mmap_fd_, &sb) < 0)
    {
        perror("fstat");
        return -2;
    }
    if (sb.st_size != mmap_len_) { return -3; }

    mem_ = ::mmap(NULL, mmap_len_, PROT_READ | PROT_WRITE, MAP_SHARED, mmap_fd_, 0);
    if (mem_ == MAP_FAILED)
    {
        perror("mmap");
        return -4;
    }

    char * start = InitHeader(reinterpret_cast<char*>(mem_));
    char * end = start + mmap_len_;
    buf_.reset (new fx::base::RingBuffer(start, end, true));
    return 0;
}

int fx::base::ProcessBus::Connect()
{
    assert (not Inited());
    std::string mmap_name = mmap_filename();

    bool reuse_old_data = true;
    //First only try to open
    mmap_fd_ = open(mmap_name.c_str(), O_RDWR);
    if (mmap_fd_ < 0)
    {
        if (errno != ENOENT)
        {
            perror("open mmap failed");
            return -1;
        }
        else
        {
            //Then try to create
            mmap_fd_ = open(mmap_name.c_str(), O_RDWR | O_CREAT | O_EXCL, 0666);
            if (mmap_fd_ < 0)
            {
                //No matter what happens, we exit
                perror("create mmap file failed");
                return -2;
            }
            //Then resize the new created mmap file
            ::ftruncate(mmap_fd_, mmap_len_);
            reuse_old_data = false;
        }
    }

    assert (mmap_fd_ >= 0);
    mem_ = ::mmap(NULL, mmap_len_, PROT_READ | PROT_WRITE, MAP_SHARED, mmap_fd_, 0);
    if (mem_ == MAP_FAILED)
    {
        perror("mmap");
        return -3;
    }

    char * start = InitHeader(reinterpret_cast<char*>(mem_));
    char * end = start + mmap_len_;
    buf_.reset (new fx::base::RingBuffer(start, end, reuse_old_data)); //discard old data
    return 0;
}

int fx::base::ProcessBus::Listen()
{
    assert (not Inited());
    std::string mmap_name = mmap_filename();

    mmap_fd_ = open(mmap_name.c_str(), O_RDWR | O_CREAT, 0666);
    if (mmap_fd_ < 0)
    {
        perror("open mmap");
        return -1;
    }
    ::ftruncate(mmap_fd_, mmap_len_);

    mem_ = ::mmap(NULL, mmap_len_, PROT_READ | PROT_WRITE, MAP_SHARED, mmap_fd_, 0);
    if (mem_ == MAP_FAILED)
    {
        perror("mmap");
        return -2;
    }

    char * start = InitHeader(reinterpret_cast<char*>(mem_));
    char * end = start + mmap_len_;
    buf_.reset (new fx::base::RingBuffer(start, end, false)); //discard old data
    return 0;
}

std::string fx::base::ProcessBus::mmap_filename() const
{
    std::ostringstream oss;
    oss << filepath_ << "/" << bus_id_ << ".mmap";
    return oss.str();
}
