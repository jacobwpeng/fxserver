/*
 * =====================================================================================
 *
 *       Filename:  process_bus_manager.cc
 *        Created:  08/20/14 14:40:41
 *         Author:  peng wang
 *          Email:  pw2191195@gmail.com
 *    Description:  
 *
 * =====================================================================================
 */

#include "process_bus_manager.h"
#include <glog/logging.h>
#include <boost/typeof/typeof.hpp>
#include <boost/foreach.hpp>
#include <boost/property_tree/ptree.hpp>
#include <boost/property_tree/xml_parser.hpp>

namespace fx
{
    namespace base
    {
        ProcessBusManager::ProcessBusManager()
        {
        }

        ProcessBusManager::~ProcessBusManager()
        {
            BOOST_FOREACH(const BusMap::value_type& v, bus_map_)
            {
                delete v.second;
            }
        }

        int ProcessBusManager::Init(const std::string& conf_path)
        {
#if 0
            <ProcessBus>
                <Bus id="10000" size="10000000" path="/tmp" />
                <Bus id="10001" size="10000000" path="/tmp" />
            </ProcessBus>
#endif
            using boost::property_tree::ptree;
            ptree pt;
            try
            {
                boost::property_tree::read_xml(conf_path, pt, boost::property_tree::xml_parser::no_comments);
                BOOST_FOREACH(const ptree::value_type & bus, pt.get_child("ProcessBus"))
                {
                    if (bus.first != "Bus") continue;
                    unsigned id = bus.second.get<unsigned>("<xmlattr>.id");
                    unsigned size = bus.second.get<unsigned>("<xmlattr>.size");
                    std::string path = bus.second.get<std::string>("<xmlattr>.path");
                    ProcessBus * pb = new ProcessBus(id, size, path);
                    assert (bus_map_.find(id) == bus_map_.end());
                    bus_map_[id] = pb;
                }
            }
            catch(boost::property_tree::ptree_error & error)
            {
                LOG(ERROR) << "ProcessBusManage::Init failed, " << error.what();
                return -1;
            }
            return 0;
        }
        int ProcessBusManager::Connect(unsigned bus_id)
        {
            ProcessBus * pb = FindBus(bus_id);
            if (pb == NULL) return -1;
            else return pb->Connect();
        }

        int ProcessBusManager::Listen(unsigned bus_id)
        {
            ProcessBus * pb = FindBus(bus_id);
            if (pb == NULL) return -1;
            else return pb->Listen();
        }

        int ProcessBusManager::TryRecover(unsigned bus_id)
        {
            ProcessBus * pb = FindBus(bus_id);
            if (pb == NULL) return -1;

            int ret = pb->TryRecover();
            LOG_IF(WARNING, ret != 0) << "TryRecover failed, bus_id = " << bus_id << ", ret = " << ret;
            return ret;
        }

        int ProcessBusManager::Write(unsigned bus_id, const char * buf, int len)
        {
            ProcessBus * pb = FindBus(bus_id);
            if (pb == NULL) return -1;

            bool ok = pb->Write(buf, len);
            return ok ? 0 : -2;
        }

        int ProcessBusManager::Read(unsigned bus_id, char ** pbuf, int * plen)
        {
            ProcessBus * pb = FindBus(bus_id);
            if (pb == NULL) return -1;

            char * data = pb->Read(plen);
            *pbuf = data;
            return 0;
        }

        ProcessBus * ProcessBusManager::FindBus(unsigned bus_id)
        {
            BOOST_AUTO(iter, bus_map_.find(bus_id));
            ProcessBus * pb = (iter == bus_map_.end()) ? NULL : iter->second;
            LOG_IF(WARNING, pb == NULL) << "Cannot find bus_id " << bus_id;
            return pb;
        }
    }
}
