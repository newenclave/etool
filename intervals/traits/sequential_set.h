#ifndef ETOOL_INTERVALS_TRAITS_SEQUENTIAL_SET_H
#define ETOOL_INTERVALS_TRAITS_SEQUENTIAL_SET_H

#include <vector>
#include <algorithm>

#include "etool/intervals/interval.h"

namespace etool { namespace intervals { namespace traits {

    template <typename ContainerInfo>
    struct sequential_set {

        using value          = typename ContainerInfo::value;
        using position       = typename ContainerInfo::position;
        using container      = typename ContainerInfo::container;

        using comparator     = typename position::cmp;

        using iterator       = typename container::iterator;
        using const_iterator = typename container::const_iterator;

        struct iterator_access {

            template <typename IterT>
            static constexpr
            IterT &get( IterT &itr )
            {
                return itr;
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
            return std::upper_bound( c.begin( ), c.end( ), p, comparator( ) );
        }

        static
        const_iterator upper_bound( const container &c, const position &p )
        {
            return std::upper_bound( c.begin( ), c.end( ), p, comparator( ) );
        }

        static
        iterator lower_bound( container &c, const position &p )
        {
            return std::lower_bound( c.begin( ), c.end( ), p, comparator( ) );
        }

        static
        const_iterator lower_bound( const container &c, const position &p )
        {
            return std::lower_bound( c.begin( ), c.end( ), p, comparator( ) );
        }

        static
        iterator erase( container &c, iterator from, iterator to )
        {
            return c.erase( from, to );
        }

//        static
//        iterator insert( container &c, position p )
//        {
//            return c.emplace( std::move(p) ).first;
//        }

        static
        iterator insert_hint( container &c, iterator h, position p )
        {
            return c.emplace( h, std::move(p) );
        }

    };


}}}

#endif // VECTOR_SET_H
