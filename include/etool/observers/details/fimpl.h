#ifndef ETOOL_OBSERVERS_DETAILS_FIMPL_H
#define ETOOL_OBSERVERS_DETAILS_FIMPL_H

#include <list>
#include <mutex>

namespace etool { namespace observers { namespace details {

    template <typename SlotTraits, typename MutexType>
    struct fimpl {

        using slot_traits = SlotTraits;
        using slot_type = typename slot_traits::value_type;
        using mutex_type = MutexType;
        using guard_type = std::lock_guard<mutex_type>;

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

        list_type list_;

        mutable mutex_type list_lock_;
        std::size_t id_ = 100;
        std::uint32_t current_enter_ = 0;

        ~fimpl()
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
            guard_type lck(list_lock_);
            remove_by_index(list_, itr);
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

        static void clear_list(list_type& lst)
        {
            list_iterator b = lst.begin();
            while (b != lst.end()) {
                b = itr_erase(lst, b);
            }
        }

        void clear_unsafe()
        {
            list_.clear();
        }

        void clear()
        {
            guard_type lock(list_lock_);
            clear_unsafe();
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
            guard_type l(list_lock_);
            std::size_t next = next_id();
            list_.emplace_back(slot_info(std::move(call), next));
            return next;
        }

        template <typename... Args>
        void call(Args&&... args)
        {
            list_type tmp;
            {
                guard_type l(list_lock_);
                tmp = list_;
            }

            list_iterator b(tmp.begin());
            while (b != tmp.end()) {
                if (!slot_traits::expired(b->slot_)) {
                    slot_traits::exec(b->slot_, std::forward<Args>(args)...);
                    ++b;
                } else {
                    b = itr_erase(tmp, b);
                }
            }
        }
    };
}}}

#endif
