#ifndef SET_H
#define SET_H

#endif // SET_H
#ifndef ETOOL_INTERVALS_SET_H
#define ETOOL_INTERVALS_SET_H

#include "etool/intervals/tree.h"
#include "etool/intervals/traits/std_set.h"

namespace etool { namespace intervals {

    template <typename KeyT, typename Comp = std::less<KeyT> >
    class set: public tree<traits::std_set<KeyT, Comp> > {

        using parent_type   = tree<traits::std_set<KeyT, Comp> >;
        using interval_type = typename parent_type::interval_type;

    public:

        using data_type         = KeyT;
        using iterator          = typename parent_type::iterator;
        using const_iterator    = typename parent_type::const_iterator;

        iterator insert( const data_type& k )
        {
            return insert(interval_type::degenerate( k ));
        }

        iterator insert( interval_type k )
        {
            return parent_type::insert_impl(std::move(k) );
        }

        iterator merge( const data_type& k )
        {
            return merge(interval_type::degenerate( k ));
        }

        iterator merge( interval_type k )
        {
            return parent_type::merge_impl( std::move(k) );
        }

        iterator cut( const data_type& k )
        {
            return cut(interval_type::degenerate( k ));
        }

        iterator cut( interval_type k )
        {
            return parent_type::cut_impl( std::move(k) );
        }
    };
}}

#endif // MAP_H
