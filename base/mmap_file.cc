/*
 * =====================================================================================
 *
 *       Filename:  mmap_file.cc
 *        Created:  08/23/14 09:56:34
 *         Author:  peng wang
 *          Email:  pw2191195@gmail.com
 *    Description:  
 *
 * =====================================================================================
 */

#include "mmap_file.h"
#include <sys/types.h>
#include <sys/stat.h>
#include <sys/mman.h>
#include <fcntl.h>
#include <unistd.h>
#include <errno.h>
#include <cstdio>
#include <cassert>
#include <memory.h>

namespace fx
{
    namespace base
    {
        MMapFile::MMapFile(const std::string& path, size_t size, int flags)
            :path_(path), size_(size), fd_(-1), start_(NULL), newly_created_(false)
        {
            int open_flags = O_RDWR;
            int fd = ::open(path.c_str(), open_flags, 0666);
            if (fd < 0)
            {
                if (errno != ENOENT or not (flags & create_if_not_exists))
                {
                    perror("open");
                    return;
                }
            }

            if (fd < 0)
            {
                open_flags |= O_CREAT;
                open_flags |= O_EXCL;
                fd = ::open(path.c_str(), open_flags, 0666);
                if (fd < 0)
                {
                    perror("open create");
                    return;
                }
                newly_created_ = true;
            }

            assert (fd >= 0);
            if ((flags & truncate) or newly_created_)
            {
                ::ftruncate(fd, size_);
            }
            else
            {
                struct stat sb;
                if (fstat(fd, &sb) < 0)
                {
                    close(fd);
                    perror("fstat");
                    return;
                }
                size_ = sb.st_size;
            }

            void * mem = ::mmap(NULL, size_, PROT_READ | PROT_WRITE, MAP_SHARED, fd, 0);
            if (mem == MAP_FAILED)
            {
                close(fd);
                perror("mmap");
                return;
            }
            if (flags & zero_clear)
            {
                ::memset(mem, 0x0, size_);
            }
            fd_ = fd;
            start_ = mem;
        }

        MMapFile::~MMapFile()
        {
            if (Inited())
            {
                ::munmap(start_, size_);
                close(fd_);

                start_ =  NULL;
                fd_ = -1;
            }
        }

        bool MMapFile::Inited() const
        {
            return fd_ >= 0 and start_ != NULL;
        }

        void * MMapFile::start() const
        {
            assert (Inited());
            return start_;
        }

        void * MMapFile::end() const
        {
            assert (Inited());
            return reinterpret_cast<char*>(start_) + size_;
        }

        size_t MMapFile::size() const
        {
            assert (Inited());
            return size_;
        }
    }
}
