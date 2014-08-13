/*
 * =====================================================================================
 *       Filename:  fx_object_pool.hpp
 *        Created:  13:40:16 Apr 16, 2014
 *         Author:  jacobwpeng
 *          Email:  jacobwpeng@tencent.com
 *    Description:  for frequently construct and destroyed objects
 *                  better for large object
 *                  Never return memory until destructor called
 *
 * =====================================================================================
 */

#ifndef  __FX_OBJECT_POOL_HPP__
#define  __FX_OBJECT_POOL_HPP__

#include <boost/pool/pool.hpp>

namespace fx
{
    template<typename T>
    class ObjectPool
    {
        public:
            ObjectPool()
                :p_( sizeof(T) )
            { 
            }

            ~ObjectPool()
            { 
            }

            T * Construct()
            {
                return new (p_.malloc()) T;
            }

            void Destroy(T* ptr)
            {
                ptr->~T();
                p_.free(ptr);
            }

        private:
            boost::pool<> p_;
    };
}

#endif   /* ----- #ifndef __FX_OBJECT_POOL_HPP__----- */
