#ifndef ETOOL_OBSERVERS_BASE_H
#define ETOOL_OBSERVERS_BASE_H

#include <list>
#include <set>

#include <memory>
#include <mutex>

#include "etool/observers/details/impl.h"
#include "etool/observers/traits/simple.h"

#include "etool/observers/scoped-subscription.h"
#include "etool/observers/subscription.h"

namespace etool { namespace observers {

    template <typename ImplType>
    class base {

        using this_type = base<ImplType>;

    public:
        using slot_traits = typename ImplType::slot_traits;
        using slot_type = typename slot_traits::value_type;

    private:
        using impl = ImplType;
        using mutex_type = typename impl::mutex_type;
        using guard_type = std::lock_guard<mutex_type>;

        typedef std::shared_ptr<impl> impl_sptr;
        typedef std::weak_ptr<impl> impl_wptr;

        struct unsubscriber : subscription::unsubscriber {

            using base_type = base<ImplType>;

            using impl_sptr = typename base_type::impl_sptr;
            using impl_wptr = typename base_type::impl_wptr;

            unsubscriber(impl_wptr p, std::size_t k)
                : parent(p)
                , key(k)
            {
            }

            std::uintptr_t data()
            {
                return reinterpret_cast<std::uintptr_t>(parent.lock().get());
            }

            void run()
            {
                if (auto lock = parent.lock()) {
                    lock->unsubscribe(key);
                }
            }

            impl_wptr parent;
            std::size_t key;
        };

        impl& get_impl()
        {
            return *impl_;
        }

        impl_wptr get_impl_wptr()
        {
            return impl_;
        }

        const impl& get_impl() const
        {
            return *impl_;
        }

    public:
        using subscription = observers::subscription;
        using scoped_subscription = observers::scoped_subscription;

        friend class observers::subscription;
        friend class observers::scoped_subscription;

        typedef subscription connection;

        base()
            : impl_(std::make_shared<impl>())
        {
        }

        base(const base&) = delete;
        base& operator=(const base&) = delete;
        base(base&& o)
        {
            impl_.swap(o.impl_);
        }

        base& operator=(base&& o)
        {
            impl_.swap(o.impl_);
            return *this;
        }

        ~base() = default;

        subscription subscribe(slot_type call)
        {
            size_t next = get_impl().connect(call);
            auto us = std::make_shared<unsubscriber>(get_impl_wptr(), next);
            return subscription(us);
        }

        scoped_subscription subscribe_scoped(slot_type call)
        {
            return scoped_subscription(subscribe(std::move(call)));
        }

        subscription connect(slot_type call)
        {
            return subscribe(call);
        }

        void unsubscribe(subscription& cc)
        {
            cc.unsubscribe();
        }

        void disconnect(subscription& cc)
        {
            unsubscribe(cc);
        }

        void unsubscribe_all()
        {
            get_impl().clear();
        }

        void disconnect_all_slots()
        {
            unsubscribe_all();
        }

        template <typename... Args>
        void operator()(Args&&... args)
        {
            get_impl().call(std::forward<Args>(args)...);
        }

    private:
        impl_sptr impl_;
    };

}}

#endif // SIMPLE_H
