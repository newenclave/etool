#ifndef ETOOL_TREES_TRIE_NODES_ARRAY_H
#define ETOOL_TREES_TRIE_NODES_ARRAY_H

#include <algorithm>
#include <memory>
#include <vector>

namespace etool { namespace trees { namespace trie { namespace nodes {

    template <typename KeyT, typename ValueT, typename Comp = std::less<KeyT>>
    class array {
    public:
        using key_type = KeyT;
        using value_type = ValueT;
        using value_ptr = std::unique_ptr<value_type>;

    private:
        using this_type = array<key_type, value_type, Comp>;
        struct key_data {

            key_type key;
            this_type value;

            key_data(const key_data& other) = delete;
            key_data& operator=(const key_data& other) = delete;

            explicit key_data(const key_type& k)
                : key(k)
            {
            }

            key_data(key_data&& other)
                : key(std::move(other.key))
                , value(std::move(other.value))
            {
            }

            key_data& operator=(key_data&& other)
            {
                key = std::move(other.key);
                value = std::move(other.value);
                return *this;
            }

            bool equal_keys(const key_type& rh) const
            {
                Comp compare;
                return (!compare(key, rh)) && (!compare(rh, key));
            }
        };

        struct cmp {
            bool operator()(const key_data& d, const key_type& k) const
            {
                Comp compare;
                return compare(d.key, k);
            }
        };

        using container = std::vector<key_data>;

    public:
        using iterator = typename container::iterator;
        using const_iterator = typename container::const_iterator;

        void erase(const_iterator itr)
        {
            next_.erase(itr);
        }

        bool is_end(const_iterator itr) const
        {
            return itr == next_.end();
        }

        array* get_node(iterator itr)
        {
            return &itr->value;
        }

        const array* get_node(const_iterator itr) const
        {
            return &itr->value;
        }

        iterator get(const key_type& k)
        {
            if (next_.empty()) {
                return next_.end();
            }
            auto f = std::lower_bound(next_.begin(), next_.end(), k, cmp());
            if (f != next_.end()) {
                return (f->equal_keys(k)) ? f : next_.end();
            } else {
                return next_.end();
            }
        }

        const_iterator get(const key_type& k) const
        {
            if (next_.empty()) {
                return next_.end();
            }
            auto f = std::lower_bound(next_.begin(), next_.end(), k, cmp());
            if (f != next_.end()) {
                return (f->equal_keys(k)) ? f : next_.end();
            } else {
                return next_.end();
            }
        }

        iterator set(const key_type& k)
        {
            if (next_.empty()) {
                next_.emplace_back(key_data(k));
                return next_.begin();
            } else {
                auto f = std::lower_bound(next_.begin(), next_.end(), k, cmp());
                if (f != next_.end() && f->equal_keys(k)) {
                    return f;
                } else {
                    auto nf = next_.emplace(f, key_data(k));
                    return nf;
                }
            }
        }

        std::size_t remove(const array* node)
        {
            for (auto b = next_.begin(); b != next_.end(); ++b) {
                if (&b->value == node) {
                    next_.erase(b);
                    return 1;
                }
            }
            return 0;
        }

        void set_value(value_type val)
        {
            val_.reset(new value_type(std::move(val)));
        }

        void reset_value()
        {
            val_.reset();
        }

        bool empty() const
        {
            return next_.empty();
        }

        value_type* value()
        {
            return val_.get();
        }

        const value_type* value() const
        {
            return val_.get();
        }

    private:
        value_ptr val_;
        container next_;
    };
}}}}

#endif // ETOOL_TREES_TRIE_NODES_ARRAY_H
