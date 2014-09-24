/*
 * =====================================================================================
 *
 *       Filename:  process_bus_manager.h
 *        Created:  08/20/14 14:30:07
 *         Author:  peng wang
 *          Email:  pw2191195@gmail.com
 *    Description:  
 *
 * =====================================================================================
 */

#ifndef  __PROCESS_BUS_MANAGER_H__
#define  __PROCESS_BUS_MANAGER_H__

#include <map>
#include <string>
#include <memory>
#include <boost/function.hpp>
#include <boost/noncopyable.hpp>
#include "process_bus.h"

namespace fx
{
    namespace base
    {
        namespace bus
        {
            class ProcessBusManager : boost::noncopyable
            {
                public:

                    ProcessBusManager();
                    ~ProcessBusManager();

                    int Init(const std::string& conf_path);
                    int Connect(unsigned bus_id);
                    int Listen(unsigned bus_id);
                    int TryRecover(unsigned bus_id);

                    int Write(unsigned bus_id, const char * buf, int len);
                    int Read(unsigned bus_id, char ** pbuf, int * plen);

                private:
                    typedef std::unique_ptr<ProcessBus> ProcessBusPtr;
                    typedef std::map<unsigned, ProcessBusPtr> BusMap;

                    ProcessBusPtr FindBus(unsigned bus_id);
                private:
                    BusMap bus_map_;
            };
        }
    }
}

#endif   /* ----- #ifndef __PROCESS_BUS_MANAGER_H__  ----- */
