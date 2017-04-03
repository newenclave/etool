#ifndef ETOOL_CHACHE_SIMPLE_H
#define ETOOL_CHACHE_SIMPLE_H

#include <queue>
#include <mutex>

#include "etool/cache/traits/shared.h"
#include "etool/cache/traits/unique.h"
#include "etool/cache/traits/raw.h"

namespace etool { namespace cache {

    template <typename T,
              typename MutexType  = std::mutex,
              typename ValueTrait = traits::shared<T> >
    class simple
    {
        typedef std::lock_guard<MutexType> locker_type;

        void clear_unsafe( )
        {
            while( !cache_.empty( ) ) {
                ValueTrait::destroy( std::move(cache_.front( )) );
                cache_.pop( );
            }
        }

    public:

        typedef typename ValueTrait::value_type value_type;
        typedef MutexType                       mutex_type;

        simple( size_t maximum )
            :maximum_(maximum)
        { }

        simple( )
            :maximum_(0)
        { }

        ~simple( )
        {
            clear_unsafe( );
        }

        void clear( )
        {
            locker_type l(cache_lock_);
            clear_unsafe( );
        }

        template <typename ...Args>
        value_type get( Args && ...args )
        {
            locker_type l(cache_lock_);
            if( cache_.empty( ) ) {
                return ValueTrait::create(std::forward<Args>(args)...);
            } else {
                value_type n = std::move(cache_.front( ));
                cache_.pop( );
                return n;
            }
        }

        void push( value_type val )
        {
            locker_type l(cache_lock_);
            if( (maximum_ > 0) && (cache_.size( ) < maximum_) ) {
                cache_.emplace( std::move(val) );
            }
        }

        size_t size( ) const
        {
            locker_type l(cache_lock_);
            return cache_.size( );
        }

    private:

        size_t                 maximum_;
        std::queue<value_type> cache_;
        mutable mutex_type     cache_lock_;
    };

}}

#endif // SRPC_COMMON_CHACHE_SIMPLE_H
