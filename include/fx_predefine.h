/*
 * =====================================================================================
 *       Filename:  fx_predefine.h
 *        Created:  10:49:06 Mar 13, 2014
 *         Author:  jacobwpeng
 *          Email:  jacobwpeng@tencent.com
 *    Description:  predefined headers for fx framework
 *
 * =====================================================================================
 */


#ifndef  __FX_PREDEFINE_H__
#define  __FX_PREDEFINE_H__


/*-----------------------------------------------------------------------------
 *  system headers
 *-----------------------------------------------------------------------------*/
#include <sys/epoll.h>
#include <sys/socket.h>
#include <sys/types.h>
#include <sys/time.h>
#include <arpa/inet.h>
#include <fcntl.h>
#include <netinet/in.h>

/*-----------------------------------------------------------------------------
 *  standard c headers
 *-----------------------------------------------------------------------------*/
#include <stdint.h>
#include <cassert>

/*-----------------------------------------------------------------------------
 *  standard cplusplus headers
 *-----------------------------------------------------------------------------*/
#include <map>
#include <string>
#include <vector>
#include <algorithm>

/*-----------------------------------------------------------------------------
 *  boost headers
 *-----------------------------------------------------------------------------*/
#include <boost/bind.hpp>
#include <boost/format.hpp>
#include <boost/foreach.hpp>
#include <boost/optional.hpp>
#include <boost/function.hpp>
#include <boost/weak_ptr.hpp>
#include <boost/shared_ptr.hpp>
#include <boost/make_shared.hpp>
#include <boost/noncopyable.hpp>
#include <boost/lexical_cast.hpp>
#include <boost/typeof/typeof.hpp>

/*-----------------------------------------------------------------------------
 *  google-glog headers
 *-----------------------------------------------------------------------------*/
#include <glog/logging.h>

#endif   /* ----- #ifndef __FX_PREDEFINE_H__----- */

