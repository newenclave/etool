#ifndef ETOOL_OBSERVERS_SCOPED_SUBSCRIPTION_H
#define ETOOL_OBSERVERS_SCOPED_SUBSCRIPTION_H

#include "etool/observers/subscription.h"
#include <memory>

namespace etool { namespace observers {

    class scoped_subscription {

        using unsubscriber_sptr = subscription::unsubscriber_sptr;

    public:
        /// C-tors
        scoped_subscription(scoped_subscription&& o)
        {
            unsubscriber_.swap(o.unsubscriber_);
            o.reset();
        }

        scoped_subscription& operator=(scoped_subscription&& o)
        {
            if (this != &o) {
                unsubscriber_.swap(o.unsubscriber_);
                o.reset();
            }
            return *this;
        }

        scoped_subscription(subscription&& o)
            : unsubscriber_(std::move(o.unsubscriber_))
        {
        }

        scoped_subscription& operator=(subscription&& o)
        {
            unsubscribe();
            unsubscriber_.swap(o.unsubscriber_);
            return *this;
        }

        scoped_subscription() = default;
        scoped_subscription(const scoped_subscription& o) = delete;
        scoped_subscription& operator=(const scoped_subscription& o) = delete;

        scoped_subscription(const subscription& o)
            : unsubscriber_(o.unsubscriber_)
        {
        }

        /// D-tor
        ~scoped_subscription()
        {
            unsubscribe();
        }

        scoped_subscription& operator=(const subscription& o)
        {
            unsubscribe();
            unsubscriber_ = o.unsubscriber_;
            return *this;
        }

        void unsubscribe()
        {
            if (unsubscriber_) {
                unsubscriber_->run();
            }
        }

        void disconnect()
        {
            unsubscribe();
        }

        subscription release()
        {
            subscription tmp;
            unsubscriber_.swap(tmp.unsubscriber_);
            return tmp;
        }

        void reset()
        {
            if (unsubscriber_) {
                unsubscriber_sptr tmp;
                unsubscriber_.swap(tmp);
                tmp->run();
            }
        }

        void swap(scoped_subscription& other)
        {
            unsubscriber_.swap(other.unsubscriber_);
        }

    private:
        unsubscriber_sptr unsubscriber_;
    };

}}

#endif // SCOPEDSUBSCRIPTION_H
