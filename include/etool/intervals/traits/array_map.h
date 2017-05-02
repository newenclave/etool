#ifndef ETOOL_INTERVALS_TRAITS_ARRAY_MAP_H
#define ETOOL_INTERVALS_TRAITS_ARRAY_MAP_H

#include <set>
#include <vector>
#include "etool/intervals/interval.h"

namespace etool { namespace intervals { namespace traits {

    template <typename KeyT, typename ValueT, typename Comparator,
              typename AllocT>
    struct array_map {

        using interval_type     = interval<KeyT, Comparator>;
        using key_type          = interval_type;
        using value_type        = std::pair<key_type, ValueT>;

        using allocator_type    = AllocT;
        using array_type        = std::vector<value_type, allocator_type>;
        using iterator          = typename array_type::iterator;
        using const_iterator    = typename array_type::const_iterator;

        struct comparator {
            bool operator ( )( const value_type &val, const key_type &k ) const
            {
                const typename interval_type::cmp_not_overlap cmp;
                return cmp( val.first, k );
            }

            bool operator ( )( const key_type &k, const value_type &val ) const
            {
                const typename interval_type::cmp_not_overlap cmp;
                return cmp( k, val.first );
            }

            bool operator ( )( const value_type &lh,
                               const value_type &rh ) const
            {
                const typename interval_type::cmp_not_overlap cmp;
                return cmp( lh.first, rh.first );
            }
        };

        using set_cmp = comparator;

        struct container_type {

            using iterator       = typename array_type::iterator;
            using const_iterator = typename array_type::const_iterator;

            iterator begin( )
            {
                return arr_.begin( );
            }

            const_iterator begin( ) const
            {
                return arr_.begin( );
            }

            iterator end( )
            {
                return arr_.end( );
            }

            const_iterator cbegin( ) const
            {
                return arr_.begin( );
            }

            const_iterator cend( ) const
            {
                return arr_.end( );
            }

            const_iterator end( ) const
            {
                return arr_.end( );
            }

            size_t size( ) const
            {
                return arr_.size( );
            }

            size_t empty( ) const
            {
                return arr_.empty( );
            }

            void swap( container_type &other )
            {
                arr_.swap( other.arr_ );
            }

            iterator lower_bound( const key_type &val )
            {
                return std::lower_bound( begin( ), end( ), val, set_cmp( ) );
            }

            iterator upper_bound( const key_type &val )
            {
                return std::upper_bound( begin( ), end( ), val, set_cmp( ) );
            }

            iterator find( const key_type &val )
            {
                return std::binary_search( begin( ), end( ), val, set_cmp( ) );
            }

            const_iterator find( const key_type &val ) const
            {
                return std::binary_search( begin( ), end( ), val, set_cmp( ) );
            }

            iterator emplace_hint( const_iterator where, value_type val )
            {
                if( where != cend( )) {
                    const set_cmp cless;
                    const_iterator lb;
                    if( cless( val, where->first ) ) {
                        lb = std::lower_bound( cbegin( ), where,
                                               val.first, set_cmp( ) );
                    } else {
                        lb = std::lower_bound( where, cend( ),
                                               val.first, set_cmp( ) );
                    }
                    return arr_.emplace( lb, std::move(val) );
                } else {
                    return arr_.emplace( arr_.end( ), std::move( val ) );
                }
            }

            iterator erase( const_iterator where )
            {
                return arr_.erase( where );
            }

            iterator erase( const_iterator from, const_iterator to )
            {
                return arr_.erase( from, to );
            }

            array_type arr_;
        };

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
