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

#include <vector>
#include <boost/intrusive/list.hpp>

namespace bi = boost::intrusive;
typedef bi::list_base_hook< bi::link_mode< bi::auto_unlink> > auto_unlink_hook;

namespace detail
{
    template<typename T>
    struct Dummy : public auto_unlink_hook
    {
#ifndef NDEBUG
        Dummy() :inited_(false){}
#endif
        char buf[sizeof(T)];

        T* Init()
        {
#ifndef NDEBUG
            assert( not inited_ );
            T * ptr = new(buf)T;
            inited_ = true;
            return ptr;
#else
            return new(buf)T;
#endif
        }

        void Destroy()
        {
#ifndef NDEBUG
            assert( inited_ );
            inited_ = false;
#endif
            reinterpret_cast<T*>(buf)->~T();
        }
#ifndef NDEBUG
        private:
            bool inited_;
#endif
    };

}

namespace fx
{
    template<typename T>
    class ObjectPool 
    {
        public:
            ObjectPool(size_t alloca_num_each_time = 16)
            :kAllocaNum(alloca_num_each_time)
            {}
            ~ObjectPool()
            {
                while( not used_.empty() )
                {
                    used_.front().Destroy();
                    used_.pop_front();
                }

                for( size_t idx = 0; idx != ptrs_.size(); ++idx )
                {
                    delete [] ptrs_[idx];
                }
            }

            T * Construct()
            {
                if( reserved_.empty() )
                {
                    DummyType * arr = new DummyType[kAllocaNum];
                    for( size_t idx = 0; idx != kAllocaNum; ++ idx )
                    {
                        reserved_.push_front( arr[idx] );
                    }
                    ptrs_.push_back( arr );
                }
                DummyType * ptr = &( reserved_.front() );
                reserved_.pop_front();
                assert( ptr->is_linked() == false );
                used_.push_front( *ptr );

                return ptr->Init();
            }

            void Destroy( T * p )
            {
                DummyType * addr = reinterpret_cast<DummyType*>( (char*)p - offsetof(DummyType, buf) );
                addr->unlink();
                addr->Destroy();
                reserved_.push_front( *addr );
            }

        private:
            typedef detail::Dummy<T> DummyType;
            typedef bi::list<DummyType , bi::constant_time_size<false> > DummyList;
            typedef std::vector<DummyType*> DummyTypePtrList;
            DummyList reserved_;
            DummyList used_;
            DummyTypePtrList ptrs_;
            const size_t kAllocaNum;
    };
}

#endif   /* ----- #ifndef __FX_OBJECT_POOL_HPP__----- */
