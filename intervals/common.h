#ifndef ETOOL_INTERVALS_COMMON_H
#define ETOOL_INTERVALS_COMMON_H

#include <utility>
#include <cstdint>
#include <cstdlib>
#include <algorithm>

#include "etool/intervals/flags.h"

namespace etool { namespace intervals {

    template <typename TraitT>
    class common {

        using trait_type      = TraitT;
        using key             = typename trait_type::key;
        using position        = typename trait_type::position;
        using container       = typename trait_type::container;
        using iterator_access = typename trait_type::iterator_access;

        using iterator        = typename container::iterator;
        using const_iterator  = typename container::const_iterator;
        using factory         = typename position::factory;

    public:

        template <typename ItrT>
        struct container_slice {

            using iterator = ItrT;

            container_slice(  ) = default;
            container_slice( container_slice&& ) = default;
            container_slice( const container_slice& ) = default;
            container_slice& operator = ( container_slice && ) = default;
            container_slice& operator = ( const container_slice & ) = default;

            container_slice( iterator b, iterator e )
                :data(std::move(b), std::move(e))
            { }

            iterator begin( )
            {
                return data.first;
            }

            iterator end( )
            {
                return data.second;
            }

        private:
            std::pair<iterator, iterator> data;
        };

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
            return intersection<iterator>( cont_, p );
        }

        container_slice<iterator>
        intersection( const key &lft, const key &rght )
        {
            return intersection<iterator>( cont_, position( lft, rght ) );
        }

        container_slice<iterator>
        intersection( const key &lft, const key &rght, std::uint32_t flgs )
        {
            return intersection<iterator>( cont_, position( lft, rght, flgs ) );
        }

        std::size_t size( ) const
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
        struct iter_bool {
            iter_bool( IterT i, bool in, bool bo )
                :iter(i)
                ,inside(in)
                ,border(bo)
            { }

            iter_bool( const iter_bool & )              = default;
            iter_bool( iter_bool && )                   = default;
            iter_bool &operator = ( iter_bool && )      = default;
            iter_bool &operator = ( const iter_bool & ) = default;

            IterT iter;
            bool  inside = false;
            bool  border = false;
        };

        template <typename ItrT>
        using place_pair = std::pair< iter_bool<ItrT>, iter_bool<ItrT> >;

        using iter_access = typename trait_type::iterator_access;

        using position_pair = std::pair<position, position>;

        static
        iterator find( container &cont, const key &k )
        {
            auto res = locate<iterator>( cont, factory::degenerate( k ) );
            return res.first.inside ? res.first.iter : trait_type::end(cont);
        }

        static
        const_iterator find( const container &cont, const key &k )
        {
            auto res = locate<const_iterator>( cont, position( k, k ) );
            return res.first.inside ? res.first.iter : trait_type::end(cont);
        }

        template <typename ItrT>
        static
        bool has_left_border( ItrT &itr, const typename TraitT::position &p )
        {
            using iter_acc = typename trait_type::iterator_access;
            ItrT prev = std::prev(itr, 1);
            return p.left_connected(*iter_acc::get(prev));
        }

        template <typename ItrT>
        static
        bool has_right_border( ItrT &itr, const typename TraitT::position &p )
        {
            using iter_acc = typename trait_type::iterator_access;
            return p.right_connected(*iter_acc::get(itr));
        }

        template <typename IterT, typename ContT>
        static
        place_pair<IterT>
        locate( ContT &cont, const position &p )
        {
            using iter       = IterT;
            using iter_acc   = typename trait_type::iterator_access;
            using iter_bool_ = iter_bool<iter>;

            auto b = trait_type::lower_bound( cont, p );

            if( b == trait_type::end( cont ) ) {
                bool border = false;
                if( b != trait_type::begin( cont ) ) {
                    border = has_left_border(b, p);
                }
                return std::make_pair( iter_bool_( b, false, border ),
                                       iter_bool_( b, false, false ) );
            }

            bool bin = false;
            bool ein = false;

            bool bor = false;
            bool eor = false;

            auto e = trait_type::upper_bound( cont, p );

            if( b != trait_type::begin(cont) ) {
                bor = has_left_border( b, p );
            }

            bin = !p.is_left_inf( ) && iter_acc::get(b)->contain( p.left( ) );

            if( e != trait_type::begin( cont ) ) {
                auto prev = std::prev( e, 1);
                if( !p.is_right_inf( ) &&
                     iter_acc::get(prev)->contain( p.right( ) ) )
                {
                    e = prev;
                    ein = true;
                }
            }

            if( e != trait_type::end(cont) ) {
                eor = has_right_border( e, p );
            }

            return std::make_pair( iter_bool_( b, bin, bor ),
                                   iter_bool_( e, ein, eor ) );
        }

        template <typename IterT>
        static
        position_pair make_first_last( place_pair<IterT> &loc,
                                       const position &pos )
        {
            position first;
            position last;

            if( loc.first.inside ) {

                std::uint32_t linc =
                        iterator_access::get(loc.first.iter)->left_flags( );

                std::uint32_t rinc = pos.is_left_close( )
                                   ? SIDE_OPEN
                                   : SIDE_CLOSE;

                first = position( iterator_access::get(loc.first.iter)->left( ),
                                  pos.left( ),
                                  linc, rinc );
            }

            if( loc.second.inside ) {

                std::uint32_t linc = pos.is_right_close( )
                                   ? SIDE_OPEN
                                   : SIDE_CLOSE;

                std::uint32_t rinc =
                        iterator_access::get(loc.second.iter)->right_flags( );

                last = position( pos.right( ),
                             iterator_access::get(loc.second.iter)->right( ),
                             linc, rinc );

                std::advance( loc.second.iter, +1 );
            }

            return std::make_pair(first, last);
        }


        template <typename IterT, typename ContT>
        static
        container_slice<IterT>
        intersection( ContT &cont, const position &p )
        {
            auto res = locate<IterT>( cont, p );
            if( res.second.inside ) {
                std::advance( res.second.iter, 1 );
            }
            return container_slice<IterT>( std::move(res.first.iter),
                                           std::move(res.second.iter) );
        }

    private:

        container cont_;
    };

}}

#endif // COMMON_H
