#ifndef ETOOL_OBSERVERS_DETAILS_IMPL_H
#define ETOOL_OBSERVERS_DETAILS_IMPL_H

#include <list>
#include <mutex>
#include <set>

namespace etool { namespace observers { namespace details {

    template <typename SlotTraits, typename MutexType>
    struct impl {

        using slot_traits = SlotTraits;
        using slot_type = typename slot_traits::value_type;
        using mutex_type = MutexType;
        using guard_type = std::lock_guard<mutex_type>;

        struct special {
            enum { unsubscribe_all = 1 };
        };

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

            typename list_iterator b(list_.begin());
            while (b != list_.end()) {
                if (is_removed(b->id_)) {
                    b = itr_erase(list_, b);
                } else {
                    if (!slot_traits::expired(b->slot_)) {
                        slot_traits::exec(b->slot_,
                                          std::forward<Args>(args)...);
                        ++b;
                    } else {
                        b = itr_erase(list_, b);
                    }
                }
            }

            if (--current_enter_ == 0) {
                clear_removed();
            }
        }
    };

}}}

#endif