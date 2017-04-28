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

        using key_type          = KeyT;
        using iterator          = typename parent_type::iterator;
        using const_iterator    = typename parent_type::const_iterator;

        iterator insert( const key_type& k )
        {
            return insert(interval_type::degenerate( k ));
        }

        iterator insert( const interval_type& k )
        {
            return parent_type::insert_impl( k );
        }

        iterator merge( const key_type& k )
        {
            return merge(interval_type::degenerate( k ));
        }

        iterator merge( const interval_type& k )
        {
            return parent_type::merge_impl( k );
        }

        iterator cut( const key_type& k )
        {
            return cut(interval_type::degenerate( k ));
        }

        iterator cut( const interval_type& k )
        {
            return parent_type::cut_impl( k );
        }
    };
}}

#endif // MAP_H
