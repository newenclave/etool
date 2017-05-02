#ifndef ETOOL_INTERVALS_TRAITS_STD_SET_H
#define ETOOL_INTERVALS_TRAITS_STD_SET_H

#include <set>
#include "etool/intervals/interval.h"

namespace etool { namespace intervals { namespace traits {

    template <typename KeyT, typename Comparator,
              typename AllocT = std::allocator<KeyT> >
    struct std_set {

        using interval_type     = interval<KeyT, Comparator>;
        using value_type        = interval_type;
        using set_cmp           = typename interval_type::cmp_not_overlap;
        using allocator_type    = AllocT;
        using container_type    = std::set<interval_type, set_cmp,
                                           allocator_type>;
        using iterator          = typename container_type::iterator;
        using const_iterator    = typename container_type::const_iterator;

        struct iterator_access {

            static
            const interval_type &key( const_iterator itr )
            {
                return *itr;
            }

            static
            interval_type &mutable_key( iterator itr )
            {
                return const_cast<interval_type &>(*itr);
            }

            static
            const interval_type &key( const value_type &val )
            {
                return val;
            }

            static
            interval_type &mutable_key( value_type &val )
            {
                return val;
            }

            static
            void copy( value_type &, const value_type & )
            {
                //to = from;
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

#endif // STD_SET_H
