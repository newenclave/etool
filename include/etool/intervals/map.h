#ifndef ETOOL_INTERVALS_MAP_H
#define ETOOL_INTERVALS_MAP_H

#include "etool/intervals/tree.h"
#include "etool/intervals/traits/std_map.h"

namespace etool { namespace intervals {
    template <typename KeyT, typename ValueT, typename Comp = std::less<KeyT> >
    class map: public tree<traits::std_map<KeyT, ValueT, Comp> > {
        using parent_type   = tree<traits::std_map<KeyT, ValueT, Comp> >;
        using interval_type = typename parent_type::interval_type;

    public:

        using data_type         = KeyT;
        using key_type          = typename parent_type::interval_type;
        using value_type        = typename parent_type::value_type;
        using iterator          = typename parent_type::iterator;
        using const_iterator    = typename parent_type::const_iterator;

        iterator insert( value_type val )
        {
            return insert_impl(std::move(val));
        }

        iterator merge( value_type val )
        {
            return parent_type::merge_impl( std::move(val) );
        }

        iterator cut( value_type val )
        {
            return parent_type::cut_impl( std::move(val) );
        }
    };
}}

#endif // MAP_H
