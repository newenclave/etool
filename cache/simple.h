#ifndef ETOOL_CHACHE_SIMPLE_H
#define ETOOL_CHACHE_SIMPLE_H

#include <queue>
#include <mutex>

#include "etool/cache/traits/shared.h"
#include "etool/cache/traits/unique.h"
#include "etool/cache/traits/raw.h"

namespace etool { namespace cache {

    template <typename T,
              typename MutexType = std::mutex,
              template <typename> class ValueTrait = traits::shared>
    class simple
    {
        typedef std::lock_guard<MutexType> locker_type;

    public:

        using element_type  = T;
        using trait_type    = ValueTrait<element_type>;
        using value_type    = typename trait_type::value_type;
        using mutex_type    = MutexType;

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
                return trait_type::create(std::forward<Args>(args)...);
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

        void clear_unsafe( )
        {
            while( !cache_.empty( ) ) {
                trait_type::destroy( std::move(cache_.front( )) );
                cache_.pop( );
            }
        }

        size_t                 maximum_;
        std::queue<value_type> cache_;
        mutable mutex_type     cache_lock_;
    };

}}

#endif // ETOOL_CHACHE_SIMPLE_H
