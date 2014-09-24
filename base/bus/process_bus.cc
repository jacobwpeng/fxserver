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
#include "mmap_file.h"

namespace fx
{
    namespace base
    {
        namespace bus
        {
            ProcessBus::ProcessBus()
                :header_(NULL)
            {
            }

            std::unique_ptr<ProcessBus> ProcessBus::RestoreFrom(const std::string& filepath, size_t size)
            {
                if (size < kHeaderSize) return NULL;

                std::unique_ptr<ProcessBus> bus(new ProcessBus);
                bus->file_.reset (new fx::base::MMapFile(filepath, size));
                if (bus->file_->Inited() == false) return NULL;

                ProcessBus::Header * header = reinterpret_cast<ProcessBus::Header*>(bus->file_->start());
                if (header->magic_number != ProcessBus::Header::kMagicNumber) return NULL;

                bus->header_ = header;
                void * start = static_cast<char*>(bus->file_->start()) + kHeaderSize;
                assert (bus->file_->size() > kHeaderSize);
                bus->buf_ = std::move(fx::base::container::RingBuffer::RestoreFrom(start, bus->file_->size() - kHeaderSize));
                return bus;
            }

            std::unique_ptr<ProcessBus> ProcessBus::CreateFrom(const std::string & filepath, size_t size)
            {
                if (size < kHeaderSize) return NULL;

                std::unique_ptr<ProcessBus> bus(new ProcessBus);
                bus->file_.reset (new fx::base::MMapFile(filepath, size, fx::base::MMapFile::create_if_not_exists | fx::base::MMapFile::truncate));
                if (bus->file_->Inited() == false) return NULL;

                ProcessBus::Header * header = reinterpret_cast<ProcessBus::Header*>(bus->file_->start());
                header->magic_number = ProcessBus::Header::kMagicNumber;
                bus->header_ = header;

                void * start = static_cast<char*>(bus->file_->start()) + kHeaderSize;
                assert (bus->file_->size() > kHeaderSize);
                bus->buf_ = std::move(fx::base::container::RingBuffer::CreateFrom(start, bus->file_->size() - kHeaderSize));
                return bus;
            }

            std::unique_ptr<ProcessBus> ProcessBus::ConnectTo(const std::string & filepath, size_t size)
            {
                if (size < kHeaderSize) return NULL;

                std::unique_ptr<ProcessBus> bus(new ProcessBus);
                bus->file_.reset (new fx::base::MMapFile(filepath, size, fx::base::MMapFile::create_if_not_exists));
                if (bus->file_->Inited() == false) return NULL;

                ProcessBus::Header * header = reinterpret_cast<ProcessBus::Header*>(bus->file_->start());
                if (bus->file_->newly_created())
                {
                    header->magic_number = ProcessBus::Header::kMagicNumber;
                }
                else if (header->magic_number != ProcessBus::Header::kMagicNumber)
                {
                    return NULL;
                }

                bus->header_ = header;
                void * start = static_cast<char*>(bus->file_->start()) + kHeaderSize;
                assert (bus->file_->size() > kHeaderSize);
                bus->buf_ = std::move(fx::base::container::RingBuffer::RestoreFrom(start, bus->file_->size() - kHeaderSize));
                return bus;
            }

            bool ProcessBus::Write(const char * buf, int len)
            {
                return buf_->Push(buf, len);
            }

            char * ProcessBus::Read(int * plen)
            {
                return buf_->Pop(plen);
            }

            size_t ProcessBus::size() const
            {
                return buf_->element_size();
            }

        }
    }
}
