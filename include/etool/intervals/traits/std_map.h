#ifndef ETOOL_INTERVALS_TRAITS_STD_MAP_H
#define ETOOL_INTERVALS_TRAITS_STD_MAP_H

#include <map>
#include "etool/intervals/interval.h"

namespace etool { namespace intervals { namespace traits {

    template <typename KeyT, typename ValueT, typename Comparator,
              typename AllocT>
    struct std_map {

        using interval_type     = interval<KeyT, Comparator>;
        using map_cmp           = typename interval_type::cmp_not_overlap;

        using allocator_type    = AllocT;
        using container_type    = std::map<interval_type, ValueT,
                                           map_cmp, allocator_type>;

        using value_type        = typename container_type::value_type;
        using iterator          = typename container_type::iterator;
        using const_iterator    = typename container_type::const_iterator;

        struct iterator_access {

            static
            const interval_type &key( const_iterator itr )
            {
                return itr->first;
            }

            static
            interval_type &mutable_key( iterator itr )
            {
                return const_cast<interval_type &>(itr->first);
            }

            static
            const interval_type &key( const value_type &val )
            {
                return val.first;
            }

            static
            interval_type &mutable_key( value_type &val )
            {
                return const_cast<interval_type &>(val.first);
            }

            static
            void copy( value_type &to, const value_type &from )
            {
                to.second = from.second;
            }

            static
            const value_type &val( const_iterator itr )
            {
                return *itr;
            }

            static
            value_type &mutable_val( iterator itr )
            {
                return *itr;
            }
        };
    };

}}}
#endif // STD_MAP_H
