#ifndef ETOOL_INTERVALS_TRAITS_ARRAY_SET_H
#define ETOOL_INTERVALS_TRAITS_ARRAY_SET_H

#include <set>
#include <vector>
#include "etool/intervals/interval.h"

namespace etool { namespace intervals { namespace traits {

    template <typename ValueT, typename Comparator>
    struct array_set {

        using interval_type     = interval<ValueT, Comparator>;
        using value_type        = interval_type;

        using array_type        = std::vector<value_type>;
        using iterator          = typename array_type::iterator;
        using const_iterator    = typename array_type::const_iterator;

        using set_cmp = typename interval_type::cmp_not_overlap;

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

            iterator lower_bound( const value_type &val )
            {
                return std::lower_bound( begin( ), end( ), val, set_cmp( ) );
            }

            iterator upper_bound( const value_type &val )
            {
                return std::upper_bound( begin( ), end( ), val, set_cmp( ) );
            }

            iterator find( const value_type &val )
            {
                return std::binary_search( begin( ), end( ), val, set_cmp( ) );
            }

            const_iterator find( const value_type &val ) const
            {
                return std::binary_search( begin( ), end( ), val, set_cmp( ) );
            }

            iterator emplace_hint( const_iterator where, value_type val )
            {

                if( where != cend( )) {
                    const set_cmp cless;
                    const_iterator lb;
                    if( cless( val, *where ) ) {
                        lb = std::lower_bound( cbegin( ), where,
                                               val, set_cmp( ) );
                    } else {
                        lb = std::lower_bound( where, cend( ),
                                               val, set_cmp( ) );
                    }
                    return arr_.emplace( lb, std::move(val) );
                } else {
                    arr_.push_back( std::move( val ) );
                    return arr_.begin( );
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
                return *itr;
            }

            static
            interval_type &mutable_key( iterator itr )
            {
                return *itr;
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
