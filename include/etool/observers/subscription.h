#ifndef ETOOL_OBSERVERS_SUBSCRIPTION_H
#define ETOOL_OBSERVERS_SUBSCRIPTION_H

#include <memory>
#include <cstdint>

#include "etool/observers/scoped-subscription.h"

namespace etool { namespace observers {

    class scoped_subscription;
    class subscription {

        friend class etool::observers::scoped_subscription;

        void reset( )
        {
            unsubscriber_.reset( );
        }

    public:

        struct unsubscriber {
            virtual ~unsubscriber( ) { }
            virtual std::uintptr_t data( ) { return 0; }
            virtual void run( ) = 0;
            typedef std::shared_ptr<unsubscriber> sptr;
        };

        typedef std::shared_ptr<unsubscriber> unsubscriber_sptr;

        subscription( unsubscriber_sptr call )
            :unsubscriber_(call)
        { }

        subscription( subscription &&o )
            :unsubscriber_(std::move(o.unsubscriber_))
        { }

        subscription &operator = ( subscription &&o )
        {
            unsubscriber_ = o.unsubscriber_;
            return *this;
        }

        subscription( const subscription &o ) = default;
        subscription &operator = ( const subscription &o ) = default;

        subscription( )
        { }

        void unsubscribe(  )
        {
            if( unsubscriber_ ) {
                unsubscriber_->run( );
            }
        }

        void disconnect(  )
        {
            unsubscribe( );
        }

        void swap( subscription &other )
        {
            unsubscriber_.swap( other.unsubscriber_ );
        }
    private:
        unsubscriber_sptr unsubscriber_;
    };

}}

#endif // SUBSCRIPTION_H
