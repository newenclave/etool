#ifndef ETOOL_INTERVALS_SET_H
#define ETOOL_INTERVALS_SET_H


#include <set>
#include <cstdint>
#include <string>
#include <sstream>

#include "etool/intervals/flags.h"
#include "etool/intervals/interval.h"

namespace etool { namespace intervals {

    template <typename T>
    class set {

    public:

        using key_type = T;
        using pos = interval<key_type>;

    private:

        using contnr = std::set<pos, typename pos::cmp>;

    public:

        using iterator               = typename contnr::iterator;
        using const_iterator         = typename contnr::const_iterator;
        using reverse_iterator       = typename contnr::reverse_iterator;
        using const_reverse_iterator = typename contnr::const_reverse_iterator;

        iterator begin( )
        {
            return cont_.begin( );
        }

        iterator end( )
        {
            return cont_.end( );
        }

        const_iterator begin( ) const
        {
            return cont_.begin( );
        }

        const_iterator end( ) const
        {
            return cont_.end( );
        }

        iterator find( key_type k )
        {
            return find( cont_, k );
        }

        const_iterator find( key_type k ) const
        {
            return find( cont_, k );
        }

        iterator merge( pos p )
        {
            return merge( cont_, p );
        }

        iterator merge( const key_type &lft, const key_type &rght )
        {
            return merge( cont_, pos(lft, rght) );
        }

        iterator merge( const key_type &uniq )
        {
            return merge( cont_, pos(uniq, uniq, intervals::INCLUDE_BOTH ) );
        }

        iterator merge( const key_type &lft, const key_type &rght,
                        std::uint32_t flags )
        {
            return merge( cont_, pos(lft, rght, flags) );
        }

        iterator insert( pos p )
        {
            return insert( cont_, p );
        }

        iterator insert( const key_type &lft, const key_type &rght )
        {
            return insert( cont_, pos(lft, rght) );
        }

        iterator insert( const key_type &uniq  )
        {
            return insert( cont_, pos(uniq, uniq, intervals::INCLUDE_BOTH) );
        }

        iterator insert( const key_type &lft, const key_type &rght,
                         std::uint32_t flags )
        {
            return insert( cont_, pos(lft, rght, flags) );
        }

        std::ostream &out( std::ostream &o ) const
        {
            bool start = true;
            for( auto &n: *this ) {
                if( !start ) {
                    o << " ";
                }
                start = false;
                n.out(o);
            }
            return o;
        }

        size_t size( ) const
        {
            return cont_.size( );
        }

    private:

        template <typename IterT>
        struct iter_bool {
            iter_bool( IterT i, bool in )
                :iter(i)
                ,isin(in)
            { }

            iter_bool( const iter_bool & )              = default;
            iter_bool( iter_bool && )                   = default;
            iter_bool &operator = ( iter_bool && )      = default;
            iter_bool &operator = ( const iter_bool & ) = default;

            IterT iter;
            bool  isin = false;
        };

        using place_pair = std::pair< iter_bool<iterator>,
                                      iter_bool<iterator> >;

        using const_place_pair = std::pair< iter_bool<const_iterator>,
                                            iter_bool<const_iterator> >;

        template <typename IterT, typename ContT>
        static
        place_pair locate( ContT &cont, pos p )
        {
            using iter_bool_ = iter_bool<IterT>;

            auto b = cont.lower_bound( p );

            if( b == cont.end( ) ) {
                return std::make_pair( iter_bool_( b, false ),
                                       iter_bool_( b, false ) );
            }

            bool bin = false;
            bool ein = false;

            auto e = cont.upper_bound( p );

            bin = b->isin( p.left( ) );

            if( e != cont.begin( ) ) {
                auto prev = std::prev( e, 1);
                if( prev->isin( p.right( ) ) ) {
                    e = prev;
                    ein = true;
                }
            }

            return std::make_pair( iter_bool_( b, bin ),
                                   iter_bool_( e, ein ) );
        }

        static
        iterator find( contnr &cont, const key_type &k )
        {
            auto res = locate<iterator>( cont, pos( k, k, INCLUDE_BOTH ) );
            return res.first.isin ? res.first.iter : cont.end( );
        }

        static
        const_iterator find( const contnr &cont, const key_type &k )
        {
            auto res = locate<const_iterator>( cont, pos( k, k ) );
            return res.first.isin ? res.first.iter : cont.end( );
        }

        iterator merge( contnr &cont, pos p )
        {
            auto res = locate<iterator>( cont, p );

            if( p.invalid( ) || p.empty( ) ) {
                return cont.end( );
            } else if( res.first.iter == cont.end( ) ) {
                return cont.emplace( std::move(p) ).first;
            } else {

                bool fin  = res.first.isin;
                bool lin  = res.second.isin;

                auto fpos = res.first.iter;
                auto lpos = res.second.iter;

                pos new_val = pos( fin ? fpos->left( )  : p.left( ),
                                   lin ? lpos->right( ) : p.right( ),
                                   INCLUDE_NONE );

                if( fin ) {
                    new_val.set_flag( fpos->is_left_included( )
                                      ? INCLUDE_LEFT
                                      : INCLUDE_NONE );
                } else {
                    new_val.set_flag( p.is_left_included( )
                                      ? INCLUDE_LEFT
                                      : INCLUDE_NONE );
                }

                if( lin ) {
                    new_val.set_flag( lpos->is_right_included( )
                                      ? INCLUDE_RIGTH
                                      : INCLUDE_NONE );
                } else {
                    new_val.set_flag( p.is_right_included( )
                                      ? INCLUDE_RIGTH
                                      : INCLUDE_NONE );
                }

                if( res.second.isin ) {
                    ++res.second.iter;
                }

                cont.erase( res.first.iter, res.second.iter );
                return cont.emplace( new_val ).first;
            }
        }

        iterator insert( contnr &cont, pos p )
        {
            auto res = locate<iterator>( cont, p );

            if( p.invalid( ) || p.empty( ) ) {
                return cont.end( );
            } else if( res.first.iter == cont.end( ) ) {
                return cont.emplace( std::move(p) ).first;
            } else {

                pos first;
                pos last;

                if( res.first.isin ) {

                    std::uint32_t linc = res.first.iter->is_left_included( )
                                       ? INCLUDE_LEFT
                                       : INCLUDE_NONE;

                    std::uint32_t rinc = p.is_left_included( )
                                       ? INCLUDE_NONE
                                       : INCLUDE_RIGTH;

                    first = pos( res.first.iter->left( ), p.left( ),
                                 linc | rinc );
                }

                if( res.second.isin ) {

                    std::uint32_t linc = p.is_right_included( )
                                       ? INCLUDE_NONE
                                       : INCLUDE_LEFT;

                    std::uint32_t rinc = res.second.iter->is_right_included( )
                                       ? INCLUDE_RIGTH
                                       : INCLUDE_NONE;

                    last = pos( p.right( ), res.second.iter->right( ),
                                linc | rinc );

                    ++res.second.iter;
                }

                cont.erase( res.first.iter, res.second.iter );

                if( res.first.isin && !first.empty( ) ) {
                    cont.emplace( first );
                }

                iterator ret = cont.emplace( p ).first;

                if( res.second.isin && !last.empty( ) ) {
                    cont.emplace( last );
                }

                return ret;
            }
        }

        contnr cont_;
    };

}}

#endif // SET_H
