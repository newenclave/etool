#ifndef ETOOL_OBSERVERS_BASE_H
#define ETOOL_OBSERVERS_BASE_H

#include <list>
#include <set>

#include <memory>
#include <mutex>

#include "etool/observers/traits/simple.h"

#include "etool/observers/scoped-subscription.h"
#include "etool/observers/subscription.h"

namespace etool { namespace observers {

    template <typename SlotType, typename MutexType>
    class base {

        using this_type = base<SlotType, MutexType>;

    public:
        using slot_traits = SlotType;
        using slot_type = typename SlotType::value_type;

    private:
        struct special {
            enum { unsubscribe_all = 1 };
        };

        using mutex_type = MutexType;
        using guard_type = std::lock_guard<mutex_type>;

        struct impl {

            using parent_type = base<SlotType, MutexType>;

            struct slot_info {
                slot_info(slot_type slot, size_t id)
                    : slot_(std::move(slot))
                    , id_(id)
                {
                }
                slot_type slot_;
                size_t id_;
            };

            using list_type = std::list<slot_info>;
            using list_iterator = typename list_type::iterator;
            using iterator_set = std::set<size_t>;

            iterator_set removed_;
            list_type list_;
            list_type added_;

            mutable mutex_type list_lock_;
            mutable mutex_type tmp_lock_;
            std::size_t id_ = 100;
            std::uint32_t current_enter_ = 0;

            ~impl()
            {
                clear_unsafe();
                // clear(); // ??? hm
            }

            static list_iterator itr_erase(list_type& lst, list_iterator itr)
            {
                slot_traits::erase(itr->slot_);
                return lst.erase(itr);
            }

            void unsubscribe(size_t itr)
            {
                guard_type lck(tmp_lock_);
                removed_.insert(itr);
                remove_by_index(added_, itr);
            }

            bool is_removed(size_t itr)
            {
                guard_type lck(tmp_lock_);
                return (removed_.erase(itr) != 0);
            }

            bool check_clean()
            {
                guard_type lck(tmp_lock_);
                if (removed_.erase(special::unsubscribe_all) != 0) {
                    clear_added_unsafe();
                    clear_main_unsafe();
                    removed_.clear();
                    return true;
                }
                return false;
            }

            static void remove_by_index(list_type& lst, size_t id)
            {
                if (lst.size() > 0) {

                    size_t min_id = lst.begin()->id_;
                    size_t max_id = lst.rbegin()->id_;

                    if ((id >= min_id) || (id <= max_id)) {
                        list_iterator b(lst.begin());
                        for (; (b != lst.end()) && (b->id_ < id); ++b) {
                        }
                        if ((b != lst.end()) && (b->id_ == id)) {
                            itr_erase(lst, b);
                        }
                    }
                }
            }

            void clear_removed()
            {
                iterator_set tmp;
                {
                    guard_type lck(tmp_lock_);
                    tmp.swap(removed_);
                }

                typename iterator_set::iterator b(tmp.begin());
                typename iterator_set::iterator e(tmp.end());

                list_iterator bl(list_.begin());

                for (; (b != e) && (bl != list_.end()); ++b) {
                    for (; (bl != list_.end()) && (bl->id_ < *b); ++bl) {
                    }
                    if ((bl != list_.end()) && (bl->id_ == *b)) {
                        bl = itr_erase(list_, bl);
                    }
                }
            }

            static void clear_list(list_type& lst)
            {
                list_iterator b = lst.begin();
                while (b != lst.end()) {
                    b = itr_erase(lst, b);
                }
            }

            void clear_added_unsafe()
            {
                clear_list(added_);
            }

            void clear_main_unsafe()
            {
                clear_list(list_);
            }

            void clear_unsafe()
            {
                clear_main_unsafe();
                clear_added_unsafe();
                removed_.clear();
            }

            void clear()
            {
                guard_type lock(tmp_lock_);
                removed_.insert(this_type::special::unsubscribe_all);
                clear_added_unsafe();
            }

            void splice_added()
            {
                guard_type lck(tmp_lock_);
                list_.splice(list_.end(), added_);
            }

            std::size_t next_id()
            {
                id_ += 2;
                if (id_ < 100) {
                    id_ = 100;
                }
                return id_;
            }

            std::size_t connect(slot_type call)
            {
                guard_type l(tmp_lock_);
                std::size_t next = next_id();
                added_.emplace_back(slot_info(std::move(call), next));
                return next;
            }

            template <typename... Args>
            void call(Args&&... args)
            {
                guard_type l(list_lock_);

                if (++current_enter_ == 1) {
                    if (check_clean()) {
                        return;
                    }
                    splice_added();
                }

                typename impl::list_iterator b(list_.begin());
                while (b != list_.end()) {
                    if (is_removed(b->id_)) {
                        b = impl::itr_erase(list_, b);
                    } else {
                        if (!slot_traits::expired(b->slot_)) {
                            slot_traits::exec(b->slot_,
                                              std::forward<Args>(args)...);
                            ++b;
                        } else {
                            b = impl::itr_erase(list_, b);
                        }
                    }
                }

                if (--current_enter_ == 0) {
                    clear_removed();
                }
            }
        };

        typedef std::shared_ptr<impl> impl_sptr;
        typedef std::weak_ptr<impl> impl_wptr;

        struct unsubscriber : subscription::unsubscriber {

            using base_type = base<SlotType, MutexType>;

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
