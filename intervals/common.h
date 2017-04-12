#ifndef ETOOL_INTERVALS_COMMON_H
#define ETOOL_INTERVALS_COMMON_H

#include "etool/intervals/operations.h"

namespace etool { namespace intervals {

    template <typename TraitT>
    class common {

        using trait_type      = TraitT;
        using key             = typename trait_type::key;
        using position        = typename trait_type::position;
        using container       = typename trait_type::container;
        using iterator        = typename container::iterator;
        using const_iterator  = typename container::const_iterator;
        using iterator_access = typename trait_type::iterator_access;

        using my_oper         = operations<trait_type>;

    public:

        template<typename IterT>
        using container_slice = typename
                                my_oper::template container_slice<IterT>;

        iterator begin( )
        {
            return trait_type::begin( cont_ );
        }

        iterator end( )
        {
            return trait_type::end( cont_ );
        }

        const_iterator begin( ) const
        {
            return trait_type::cbegin( cont_ );
        }

        const_iterator end( ) const
        {
            return trait_type::cend( cont_ );
        }

        iterator find( const key &k )
        {
            return find( cont_, k );
        }

        const_iterator find( const key &k ) const
        {
            return find( cont_, k );
        }

        iterator erase( const_iterator place )
        {
            return trait_type::erase( cont_, place );
        }

        iterator erase( const_iterator from, const_iterator to )
        {
            return trait_type::erase( cont_, from, to );
        }

        container_slice<iterator> intersection( const position &p )
        {
            return my_oper::template intersection<iterator>( cont_, p );
        }

        container_slice<iterator>
        intersection( const key &lft, const key &rght )
        {
            return my_oper::template
                   intersection<iterator>( cont_, position( lft, rght ) );
        }

        container_slice<iterator>
        intersection( const key &lft, const key &rght, std::uint32_t flgs )
        {
            return my_oper::template intersection<iterator>( cont_,
                                                 position( lft, rght, flgs ) );
        }

        size_t size( ) const
        {
            return trait_type::size( cont_ );
        }

        void clear( )
        {
            trait_type::clear( cont_ );
        }

    protected:

        ~common( ) = default;
        common(  ) = default;
        common( common && ) = default;
        common( const common & ) = default;
        common& operator = ( common && ) = default;
        common& operator = ( const common & ) = default;

        container &get_container( )
        {
            return cont_;
        }

        const container &get_container( ) const
        {
            return cont_;
        }

        template <typename IterT>
        using iter_bool = typename my_oper::template iter_bool<IterT>;

        template <typename IterT>
        using place_pair = typename my_oper::template place_pair<IterT>;

        using iter_access = typename trait_type::iterator_access;

        template <typename IterT, typename ContT>
        static
        place_pair<IterT> locate( ContT &cont, const position &p )
        {
            return my_oper::template locate<IterT>( cont, p );
        }

        static
        iterator find( container &cont, const key &k )
        {
            auto res = locate<iterator>( cont, position( k, k, INCLUDE_BOTH ) );
            return res.first.inside ? res.first.iter : trait_type::end(cont);
        }

        static
        const_iterator find( const container &cont, const key &k )
        {
            auto res = locate<const_iterator>( cont, position( k, k ) );
            return res.first.inside ? res.first.iter : trait_type::end(cont);
        }

    private:

        container cont_;
    };

}}

#endif // COMMON_H
