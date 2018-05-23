#ifndef SET_H
#define SET_H

#endif // SET_H
#ifndef ETOOL_INTERVALS_SET_H
#define ETOOL_INTERVALS_SET_H

#include "etool/intervals/traits/array_set.h"
#include "etool/intervals/traits/std_set.h"
#include "etool/intervals/tree.h"

namespace etool { namespace intervals {

    template <typename KeyT, typename Comp = std::less<KeyT>,
              typename AllocT = std::allocator<KeyT>>
    class set : public tree<traits::std_set<KeyT, Comp, AllocT>> {

        using parent_type = tree<traits::std_set<KeyT, Comp, AllocT>>;
        using key_type = typename parent_type::key_type;

    public:
        using domain_type = KeyT;
        using iterator = typename parent_type::iterator;
        using const_iterator = typename parent_type::const_iterator;

        template <typename IterT> void insert(IterT begin, IterT end)
        {
            for (; begin != end; ++begin) {
                insert(*begin);
            }
        }

        iterator insert(domain_type k)
        {
            return insert(key_type(std::move(k)));
        }

        iterator insert(key_type k)
        {
            return parent_type::insert_impl(std::move(k));
        }

        template <typename IterT> void merge(IterT begin, IterT end)
        {
            for (; begin != end; ++begin) {
                merge(*begin);
            }
        }

        iterator merge(domain_type k)
        {
            return merge(key_type(std::move(k)));
        }

        iterator merge(key_type k)
        {
            return parent_type::merge_impl(std::move(k));
        }

        template <typename IterT> void absorb(IterT begin, IterT end)
        {
            for (; begin != end; ++begin) {
                absorb(*begin);
            }
        }

        iterator absorb(domain_type k)
        {
            return absorb(key_type(std::move(k)));
        }

        iterator absorb(key_type k)
        {
            return parent_type::absorb_impl(std::move(k));
        }

        template <typename IterT> void cut(IterT begin, IterT end)
        {
            for (; begin != end; ++begin) {
                cut(*begin);
            }
        }

        iterator cut(const domain_type& k)
        {
            return cut(key_type::degenerate(k));
        }

        iterator cut(key_type k)
        {
            return parent_type::cut_impl(std::move(k));
        }
    };
}}

#endif // MAP_H
