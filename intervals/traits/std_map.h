#ifndef ETOOL_INTERVALS_TRAITS_STD_MAP_H
#define ETOOL_INTERVALS_TRAITS_STD_MAP_H

#include <map>
#include "etool/intervals/interval.h"

namespace etool { namespace intervals { namespace traits {

    template <typename KeyT, typename ValueT>
    struct std_map {

        using key       = KeyT;
        using value     = ValueT;
        using position  = intervals::interval<value>;
        using container = std::map<position, value, typename position::cmp>;

        using iterator       = typename container::iterator;
        using const_iterator = typename container::const_iterator;

        struct iterator_access {
            template <typename IterT>
            static
            auto &get( IterT itr )
            {
                return itr->first;
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
            return c.upper_bound( p );
        }

        static
        const_iterator upper_bound( const container &c, const position &p )
        {
            return c.upper_bound( p );
        }

        static
        iterator lower_bound( container &c, const position &p )
        {
            return c.lower_bound( p );
        }

        static
        const_iterator lower_bound( const container &c, const position &p )
        {
            return c.lower_bound( p );
        }

        static
        iterator erase( container &c, iterator from, iterator to )
        {
            return c.erase( from, to );
        }

        static
        iterator insert( container &c, position p, value v )
        {
            return c.emplace(std::make_pair(std::move(p), std::move(v))).first;
        }

    };

}}}

#endif // STD_MAP_H
