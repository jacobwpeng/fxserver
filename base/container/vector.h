/*
 * =====================================================================================
 *
 *       Filename:  vector.h
 *        Created:  08/23/14 18:42:47
 *         Author:  peng wang
 *          Email:  pw2191195@gmail.com
 *    Description:  
 *
 * =====================================================================================
 */

#ifndef  __VECTOR_H__
#define  __VECTOR_H__

#include <cassert>
#include <memory>
#include <boost/utility/enable_if.hpp>
#include <boost/type_traits/is_pod.hpp>

namespace fx
{
    namespace base
    {
        namespace container
        {
            template <typename T, typename Enable = void>
            class Vector;

            template <typename T>
            class Vector<T, typename boost::enable_if< boost::is_pod<T> >::type >
            {
                private:
                    struct MetaData
                    {
                        size_t magic_number;
                        size_t size;
                        size_t value_size;
                    };

                    typedef T ValueType;
                    typedef Vector<T, typename boost::enable_if< boost::is_pod<T> >::type > ThisType;

                    static const int64_t kMagicNumber = 2473338958754726831;

                public:
                    static ThisType * CreateFrom(void * buf, size_t len)
                    {
                        std::unique_ptr<ThisType> ptr(new ThisType(len));

                        MetaData * md = reinterpret_cast<MetaData*>(buf);
                        md->magic_number = ThisType::kMagicNumber;
                        md->size = 0;
                        md->value_size = sizeof(ValueType);
                        ptr->md_ = md;
                        ptr->start_ = reinterpret_cast<ValueType*>(reinterpret_cast<char *>(md) + sizeof(MetaData));
                        return ptr.release();
                    }

                    static ThisType * RecoverFrom(void * buf, size_t len)
                    {
                        std::unique_ptr<ThisType> ptr(new ThisType(len));

                        MetaData * md = reinterpret_cast<MetaData*>(buf);
                        if (md->magic_number != ThisType::kMagicNumber
                                or md->value_size != sizeof(ValueType)
                                or (md->value_size * md->size + sizeof(MetaData) > len))
                        {
                            return NULL;
                        }
                        ptr->md_ = md;
                        ptr->start_ = reinterpret_cast<ValueType*>(reinterpret_cast<char *>(md) + sizeof(MetaData));
                        return ptr.release();
                    }

                    bool CopyFrom(const ThisType * rhs)
                    {
                        if (kMaxLength < rhs->Used() + sizeof(MetaData)) return false;

                        memcpy(start_, rhs->start_, rhs->Used());
                        md_->size = rhs->md_->size;
                        return true;
                    }

                    bool push_back(ValueType v)
                    {
                        if (HasSpace())
                        {
                            *(start_ + md_->size) = v;
                            md_->size++;
                            return true;
                        }
                        else
                        {
                            return false;
                        }
                    }

                    void erase(size_t idx)
                    {
                        return erase(idx, idx+1);
                    }

                    void erase(size_t s, size_t e)
                    {
                        assert (s < e);
                        assert (s < md_->size);
                        assert (e <= md_->size);
                        ValueType * dst = start_ + s;
                        ValueType * src = start_ + e;
                        size_t len = (md_->size - e) * sizeof(ValueType);
                        memmove(dst, src, len);
                        md_->size -= (e-s);
                    }

                    ValueType pop_back()
                    {
                        assert (md_->size != 0);
                        md_->size--;
                        return *(start_ + md_->size);
                    }

                    size_t size() const
                    {
                        return md_->size;
                    }

                    ValueType & operator[](size_t idx)
                    {
                        assert (idx < md_->size);
                        return *(start_ + idx);
                    }

                    ValueType * begin() const
                    {
                        return start_;
                    }

                    ValueType * end() const
                    {
                        return start_ + md_->size;
                    }

                private:
                    Vector(size_t len)
                        :start_(NULL), md_(NULL), kMaxLength(len)
                    {
                    }

                    bool HasSpace() const
                    {
                        return Used() + sizeof(ValueType) + sizeof(MetaData) <= kMaxLength;
                    }

                    size_t Used() const
                    {
                        return md_->size * sizeof(ValueType);
                    }

                    ValueType * start_;
                    MetaData * md_;
                    const size_t kMaxLength;
            };
        }
    }
}

#endif   /* ----- #ifndef __VECTOR_H__  ----- */
