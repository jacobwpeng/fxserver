/*
 * =====================================================================================
 *       Filename:  fx_timer_wheel.cc
 *        Created:  16:05:47 Apr 16, 2014
 *         Author:  jacobwpeng
 *          Email:  jacobwpeng@tencent.com
 *    Description:  
 *
 * =====================================================================================
 */

#include "fx_timer_wheel.h"
#include <cmath>
#include <boost/foreach.hpp>
#include <glog/logging.h>

namespace fx
{
    namespace detail
    {
        TimerId retrieve_timer_id( const Timer & t )
        {
            return t.id;
        }
    }

    const unsigned TimerWheel::kWheelSlotOffset[ kWheelCount ] = { 7, 6, 6, 6, 6 }; /* 31 */
    const unsigned TimerWheel::kWheelSlotCount[ kWheelCount ] = { 128, 64, 64, 64, 64 }; /* must be pow 2 */

    TimerWheel::TimerWheel(EventLoop * loop)
        :loop_(loop), hands_idx_(0), last_timer_id_(0), last_expire_time_(0)
         , started_(false) , adjusted_(false)
    {
        for( unsigned idx = 0; idx != kWheelCount; ++idx )
        {
            wheels_[idx] = new TimerList[ kWheelSlotCount[idx] ];
        }
        memset( hands_copy_[0], 0x0, sizeof(hands_copy_[0]) );
        memset( hands_copy_[1], 0x0, sizeof(hands_copy_[1]) );

        hands_ = hands_copy_[hands_idx_];
    }

    TimerWheel::~TimerWheel()
    {
        for( TimerMap::iterator s(timers_.begin()), e(timers_.end()); s != e; ++s ) delete s->second;
    }

    int TimerWheel::NextTimeout(TimeStamp now)
    {
        if( expire_time_queue_.empty() ) return -1;
        else
        {
            TimeStamp next_expire_time = expire_time_queue_.top();

            if( next_expire_time <= now ) return 0;
            else return static_cast<int>( next_expire_time - now );
        }
    }
    
    void TimerWheel::Start(TimeStamp now)
    {
        last_expire_time_ = now;
        started_ = true;
    }

    TimerId TimerWheel::RunAfter( int interval, TimerCallback cb )
    {
        detail::SlotPos pos = FindPos( interval );
        detail::Timer * p = timer_pool_.Construct();
        p->id = ++last_timer_id_;

        if( adjusted_ ) p->expire_time = interval + last_expire_time_;
        else p->expire_time = interval;

        p->cb = cb;
        wheels_[pos.first][pos.second].push_back( *p );

        assert( timers_.find(p->id) == timers_.end() );
        timers_[p->id] = p;

        if( adjusted_ )
        {
            expire_time_queue_.push( p->expire_time );
        }

        return p->id;
    }

    TimerId TimerWheel::RunAt( TimeStamp expire_time, TimerCallback cb )
    {
        int interval = 0;
        if( last_expire_time_ < expire_time ) interval = expire_time - last_expire_time_;

        return RunAfter(interval, cb);
    }

    void TimerWheel::RemoveTimer( TimerId id )
    {
        TimerMap::iterator iter = timers_.find( id );
        assert( iter != timers_.end() );

        TimeStamp expire_time = iter->second->expire_time;

        iter->second->unlink();
        timer_pool_.Destroy( iter->second );
        timers_.erase( iter );

        bool removed = false;
        ExpireTimeQueue tmp;
        while( not expire_time_queue_.empty() )
        {
            TimeStamp ts = expire_time_queue_.top();
            expire_time_queue_.pop();
            if( ts == expire_time and not removed )
            {
                removed = true;
                continue;
            }
            tmp.push( ts );
        }
        expire_time_queue_ = tmp;
    }

    void TimerWheel::AdjustTimer( const std::vector<TimerId> & timer_ids )
    {
        assert( started_ == true );
        assert( adjusted_ == false );

        for( size_t idx = 0; idx != timer_ids.size(); ++idx )
        {
            TimerId id = timer_ids[idx];
            TimerMap::iterator iter = timers_.find( id );
            assert( iter != timers_.end() );

            detail::Timer * p = iter->second;

            int interval = p->expire_time;          /* ûStart֮ǰexpire_time����interval */
            assert( p->is_linked() );
            p->unlink();
            detail::SlotPos pos = FindPos( interval );
            wheels_[pos.first][pos.second].push_back( *(p) );
            
            p->expire_time += last_expire_time_;    /* ���ʱ��������expire_time */
            expire_time_queue_.push( p->expire_time );
        }

        adjusted_ = true;
    }

    void TimerWheel::Step( TimeStamp now, TimerCallbackList * callbacks )
    {
        assert( callbacks != NULL );
        int interval = 0;
        /* ����ʱ������Զ��ǰ */
        if( last_expire_time_ < now ) interval = now - last_expire_time_;

        bool overflow;
        detail::SlotPos pos = FindPos( interval, true, &overflow );

        unsigned wheel_pos = pos.first;
        unsigned slot_pos = pos.second;
        TimerList l;

        unsigned end_wheel_pos = wheel_pos;
        if( overflow )
        {
            /* ��������е�timer�������� */
            end_wheel_pos = kWheelCount;
        }

        for( unsigned wheel_idx = 0; wheel_idx != end_wheel_pos; ++wheel_idx )
        {
            for( unsigned slot_idx = 0; slot_idx != kWheelSlotCount[wheel_idx]; ++slot_idx )
            {
                if( not wheels_[wheel_idx][slot_idx].empty() )
                {
                    l.splice( l.end(), wheels_[wheel_idx][slot_idx] );
                }
            }
        }

        if( overflow )
        {
            /* ����Ļ������е�Timer��Ӧ����l������ */
        }
        else
        {
            /* û�������ʱ��ͣ����λ�ñ���û�н�λ */
            assert( hands_[end_wheel_pos] <= slot_pos );

            /* û���������ô�ð�end_wheel_posλ�õ� hands[end_wheel_pos]��slot_pos-1 ��Ҳ�ӽ���*/
            for( unsigned slot_idx = hands_[end_wheel_pos]; slot_idx != slot_pos; ++slot_idx )
            {
                l.splice( l.end(), wheels_[end_wheel_pos][slot_idx] );
            }

            std::vector<TimerId> timer_ids;
            TimerList::iterator s( wheels_[end_wheel_pos][slot_pos].begin() ), e( wheels_[end_wheel_pos][slot_pos].end() );

            transform( s, e, back_inserter(timer_ids), detail::retrieve_timer_id ); /* ��ȡ���slot��������timer��id */

            BOOST_FOREACH( TimerId id, timer_ids )
            {
                TimerMap::iterator iter = timers_.find(id);
                assert( iter != timers_.end() );
                detail::Timer & t = *(iter->second);

                if( t.expire_time <= now )
                {
                    t.unlink();
                    /* ��һ�����Ѿ���ʱ */
                    l.push_back(t);
                }
            }
            /* ʣ�µ����slot��������timers��Ҫ���µ���λ���� */
        }

        SwitchHands();                          /* ����λ��֮ǰ�ȵ���ָ�� */

        while( not expire_time_queue_.empty() and now >= expire_time_queue_.top() )
        {
            expire_time_queue_.pop();
        }

        std::vector<TimerId> removed_timer_ids;
        BOOST_FOREACH( detail::Timer & t, l )           /* �Ȱ����г�ʱ��timer�Ļص���������ȥ */
        {
            removed_timer_ids.push_back( t.id );
            callbacks->push_back( t.cb ); /* ��������Timer�Ļص����� */
        }

        BOOST_FOREACH( TimerId id, removed_timer_ids ) /* Ȼ������г�ʱ��Timer�ɵ� */
        {
            TimerMap::iterator iter = timers_.find( id );
            assert( iter != timers_.end() );
            timer_pool_.Destroy( iter->second );
            timers_.erase( iter );
        }

        std::vector<detail::Timer*> timers_to_adjust;
        BOOST_FOREACH( detail::Timer & t, wheels_[end_wheel_pos][slot_pos] ) /* Ȼ���ҵ���Ҫ����λ�õ�timer */
        {
            timers_to_adjust.push_back( &t );
        }

        BOOST_FOREACH( detail::Timer * p, timers_to_adjust ) /* Ȼ�󰤸�����λ�� */
        {
            assert( p->expire_time > now );
            int interval = p->expire_time - now;
            assert( p->is_linked() );
            p->unlink();
            detail::SlotPos pos = FindPos( interval );
            wheels_[pos.first][pos.second].push_back( *(p) );
        }

        last_expire_time_ = now;
    }

    detail::SlotPos TimerWheel::FindPos( int interval, bool update_hands, bool * overflow)
    {
        assert( interval >= 0 );
        if( overflow ) *overflow = false;
        int mod, res;

        unsigned new_hands[kWheelCount];
        memcpy( new_hands, hands_, sizeof(new_hands) );

        for( unsigned idx = 0; idx != kWheelCount; ++idx )
        {
            /* ���ٵ�ȡ��ͳ�����Ҫ��ÿ��wheel��2���ݴθ�slot */
            mod = interval & (kWheelSlotCount[idx] - 1);
            res = interval >> kWheelSlotOffset[idx];

            new_hands[idx] = mod + hands_[idx];
            if( mod + hands_[idx] >= kWheelSlotCount[idx] ) /* ��λ */
            {
                new_hands[idx] -= kWheelSlotCount[idx];
                ++res;
            }

            if( res == 0 )                      /* û��Ӱ�쵽��һ���wheel */
            {
                if( update_hands ) memcpy( hands_copy_[1-hands_idx_], new_hands, kWheelCount * sizeof(unsigned) ); /* ���µ�ָ��ŵ�copy���� */
                assert( idx < kWheelCount );
                assert( new_hands[idx] < kWheelSlotCount[idx] );
                return detail::SlotPos( idx, new_hands[idx] ); /* interval ������slot */
            }
            else
            {
                interval = res;
            }
        }

        /* overflows */
        if( update_hands ) memcpy( hands_copy_[1-hands_idx_], new_hands, kWheelCount * sizeof(unsigned) );
        if( overflow )*overflow = true;
        for( unsigned idx = 0; idx != kWheelCount; ++idx )
        {
            unsigned wheel_idx = kWheelCount - idx - 1; /* �Ӹߵ��ͱ��� */
            if( new_hands[wheel_idx] != 0 )     /* �����ʱ��Ӧ�÷ŵ���һ�����λ��Ϊ0��slot�� */
            {
                assert( wheel_idx < kWheelCount );
                assert( new_hands[wheel_idx] < kWheelSlotCount[wheel_idx] );
                return detail::SlotPos( wheel_idx, new_hands[wheel_idx] );
            }
        }
        assert( false );                    /* never be here */
        return detail::SlotPos();               /* suppress warnings */
    }

    void TimerWheel::SwitchHands()
    {
        hands_idx_ = 1 - hands_idx_;
        hands_ = hands_copy_[hands_idx_];
    }
}
