#ifndef ETOOL_INTERVALS_TRAITS_VECTOR_MAP_H
#define ETOOL_INTERVALS_TRAITS_VECTOR_MAP_H

#include <algorithm>
#include <map>
#include <vector>

#include "etool/intervals/interval.h"

namespace etool { namespace intervals { namespace traits {

    template <typename KeyT, typename ValueT>
    struct vector_map {

        using key               = KeyT;
        using value             = ValueT;
        using position          = intervals::interval<key>;

        using value_pair        = std::pair<position, value>;
        using container         = std::vector<value_pair>;

        using key_comparetor    = typename position::cmp;

        using iterator          = typename container::iterator;
        using const_iterator    = typename container::const_iterator;

        struct comparator {
            bool operator ( )( const value_pair &lh,
                               const value_pair &rh ) const
            {
                return key_comparetor::less( lh.first, rh.first );
            }

            bool operator ( )( const value_pair &lh,
                               const position   &rh ) const
            {
                return key_comparetor::less( lh.first, rh );
            }

            bool operator ( )( const position   &lh,
                               const value_pair &rh ) const
            {
                return key_comparetor::less( lh, rh.first );
            }
        };

        struct iterator_access {
            template <typename IterT>
            static
            const position *get( IterT itr )
            {
                return &itr->first;
            }
        };

        static
        iterator begin( container &c )
        {
            return c.begin( );
        }

        static
        const_iterator cbegin( const container &c )
        {
            return c.begin( );
        }

        static
        iterator end( container &c )
        {
            return c.end( );
        }

        static
        const_iterator cend( const container &c )
        {
            return c.end( );
        }

        static
        size_t size( const container &c )
        {
            return c.size( );
        }

        static
        void clear( container &c )
        {
            c.clear( );
        }

        static
        iterator upper_bound( container &c, const position &p )
        {
            comparator cmp;
            return std::upper_bound(c.begin( ), c.end( ), p, cmp);
        }

        static
        const_iterator upper_bound( const container &c, const position &p )
        {
            comparator cmp;
            return std::upper_bound(c.begin( ), c.end( ), p, cmp);
        }

        static
        iterator lower_bound( container &c, const position &p )
        {
            comparator cmp;
            return std::lower_bound(c.begin( ), c.end( ), p, cmp);
        }

        static
        const_iterator lower_bound( const container &c, const position &p )
        {
            comparator cmp;
            return std::lower_bound(c.begin( ), c.end( ), p, cmp);
        }

        static
        iterator erase( container &c, iterator from, iterator to )
        {
            return c.erase( from, to );
        }

//        static
//        iterator insert( container &c, position p, value v )
//        {
//            return c.emplace(std::make_pair(std::move(p), std::move(v))).first;
//        }

        static
        iterator insert_hint( container &c, iterator h, position p, value v )
        {
    //            return insert( c, std::move(p), std::move(v) );
            return c.emplace( h, std::make_pair(std::move(p), std::move(v)) );
        }
    };

}}}

#endif // VECTOR_MAP_H
