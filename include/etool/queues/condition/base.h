#ifndef ETOOL_CONDITION_QUEUE_BASE_H
#define ETOOL_CONDITION_QUEUE_BASE_H

#include <mutex>
#include <condition_variable>

#include "traits/simple.h"
#include "traits/priority.h"

namespace etool { namespace queues { namespace condition {

    enum result {
        WAIT_OK       = 0,
        WAIT_CANCELED = 1,
        WAIT_TIMEOUT  = 2,
    };

    template <typename ValueType,
              template <typename> class QueueTrait = traits::simple,
              typename MutexType = std::mutex,
              typename ConditionalType = std::condition_variable
             >
    class base {

    public:

        typedef ValueType                       value_type;
        typedef QueueTrait<value_type>          q_traits;
        typedef MutexType                       mutex_type;
        typedef std::unique_lock<mutex_type>    locker_type;
        typedef std::condition_variable         conditional_type;
        typedef typename q_traits::queue_type   queue_type;

        base( const base & )              = delete;
        base& operator = ( const base & ) = delete;

        base( )
            :cancel_(false)
        { }

        void reset( )
        {
            locker_type l(queue_lock_);
            cancel_ = false;
        }

        void clear( )
        {
            locker_type l(queue_lock_);
            q_traits::clear( queue_ );
        }

        size_t size( ) const
        {
            locker_type l(queue_lock_);
            return q_traits::size( queue_ );
        }

        bool empty( ) const
        {
            locker_type l(queue_lock_);
            return q_traits::empty( queue_ );
        }

        void cancel( )
        {
            locker_type l(queue_lock_);
            cancel_ = true;
            queue_cond_.notify_all( );
        }

        void push( value_type new_value )
        {
            locker_type l(queue_lock_);
            q_traits::push( queue_, std::move( new_value ) );
            queue_cond_.notify_one( );
        }

        template <typename Duration>
        result wait_for( value_type &out, const Duration &dur )
        {
            locker_type l(queue_lock_);
            bool wr = queue_cond_.wait_for( l, dur, not_empty(this) );
            if( wr ) {
                if( cancel_ ) {
                    return WAIT_CANCELED;
                } else {
                    std::swap(out, q_traits::front( queue_ ));
                    q_traits::pop( queue_ );
                    return WAIT_OK;
                }
            }
            return WAIT_TIMEOUT;
        }

        bool pop( value_type &out )         /// ignores "cancel"
        {
            locker_type l(queue_lock_);
            return pop_unsafe( out );
        }

        bool pop_unsafe( value_type &out ) /// ignores "cancel"
        {
            if( !q_traits::empty( queue_ ) ) {
                std::swap(out, q_traits::front( queue_ ));
                q_traits::pop( queue_ );
                return true;
            }
            return false;
        }

        result wait( value_type &out )
        {
            locker_type l(queue_lock_);
            queue_cond_.wait( l, not_empty(this) );
            if( cancel_ ) {
                return WAIT_CANCELED;
            } else {
                std::swap(out, q_traits::front( queue_ ));
                q_traits::pop( queue_ );
                return WAIT_OK;
            }
        }

        result wait0( value_type &out )
        {
            locker_type l(queue_lock_);
            if( cancel_ ) {
                return WAIT_CANCELED;
            } else if( !q_traits::empty( queue_ ) ) {
                std::swap(out, q_traits::front( queue_ ));
                q_traits::pop( queue_ );
                return WAIT_OK;
            } else {
                return WAIT_TIMEOUT;
            }
        }

        template<typename CallType>
        void for_each_unsafe( CallType call )
        {
            for( auto &c: queue_ ) {
                call( c );
            }
        }

        template<typename CallType>
        void for_each( CallType call )
        {
            locker_type l(queue_lock_);
            for_each_unsafe( std::move(call) );
        }

        bool canceled( ) const
        {
            locker_type l(queue_lock_);
            return cancel_;
        }

    private:

        struct not_empty {

            not_empty( const base *parent )
                :parent_(parent)
            { }

            not_empty &operator = (const not_empty& other)
            {
                parent_ = other.parent_;
                return *this;
            }

            bool operator ( )( ) const
            {
                return !q_traits::empty(parent_->queue_) || parent_->cancel_;
            }

            const base *parent_;
        };

        friend struct not_empty;

        bool                cancel_;
        queue_type          queue_;
        conditional_type    queue_cond_;
        mutable mutex_type  queue_lock_;
    };

}}}

#endif // BASE_H
