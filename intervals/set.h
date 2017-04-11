#ifndef ETOOL_INTERVALS_SET_H
#define ETOOL_INTERVALS_SET_H


#include <set>
#include <cstdint>
#include <string>
#include <sstream>

#include "etool/intervals/flags.h"
#include "etool/intervals/interval.h"


#include "etool/intervals/traits/std_set.h"

namespace etool { namespace intervals {

    template <typename T,
              template <typename> class PosTraitT = traits::std_set>
    class set {

    public:

        using key_type = T;
        using trait_type = PosTraitT<key_type>;

        using pos = typename trait_type::position;

    private:

        using contnr = typename trait_type::container;

    public:

        using iterator       = typename trait_type::iterator;
        using const_iterator = typename trait_type::const_iterator;

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

        iterator cut( pos p )
        {
            return cut( cont_, p );
        }

        iterator cut( const key_type &lft, const key_type &rght )
        {
            return cut( cont_, pos(lft, rght) );
        }

        iterator cut( const key_type &uniq  )
        {
            return cut( cont_, pos(uniq, uniq, intervals::INCLUDE_BOTH) );
        }

        iterator cut( const key_type &lft, const key_type &rght,
                      std::uint32_t flags )
        {
            return cut( cont_, pos(lft, rght, flags) );
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
            return trait_type::size( cont_ );
        }

        void clear( )
        {
            trait_type::clear( cont_ );
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
        place_pair locate( ContT &cont, const pos &p )
        {
            using iter_bool_ = iter_bool<IterT>;

            auto b = trait_type::lower_bound( cont, p );

            if( b == trait_type::end( cont ) ) {
                return std::make_pair( iter_bool_( b, false ),
                                       iter_bool_( b, false ) );
            }

            bool bin = false;
            bool ein = false;

            auto e = trait_type::upper_bound( cont, p );

            bin = b->contain( p.left( ) );

            if( e != trait_type::begin( cont ) ) {
                auto prev = std::prev( e, 1);
                if( prev->contain( p.right( ) ) ) {
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
            return res.first.isin ? res.first.iter : trait_type::end(cont);
        }

        static
        const_iterator find( const contnr &cont, const key_type &k )
        {
            auto res = locate<const_iterator>( cont, pos( k, k ) );
            return res.first.isin ? res.first.iter : trait_type::end(cont);
        }

        iterator merge( contnr &cont, pos p )
        {
            auto res = locate<iterator>( cont, p );

            if( p.invalid( ) || p.empty( ) ) {
                return trait_type::end( cont );
            } else if( res.first.iter == trait_type::end( cont ) ) {
                return trait_type::insert( cont, std::move(p) );
            } else {

                bool fin  = res.first.isin;
                bool lin  = res.second.isin;

                iterator left_pos  = res.first.iter;
                iterator right_pos = res.second.iter;

                pos new_val = pos( fin ? left_pos->left( )   : p.left( ),
                                   lin ? right_pos->right( ) : p.right( ),
                                   INCLUDE_NONE );

                if( fin ) {
                    new_val.set_flag( left_pos->is_left_included( )
                                      ? INCLUDE_LEFT
                                      : INCLUDE_NONE );
                } else {
                    new_val.set_flag( p.is_left_included( )
                                      ? INCLUDE_LEFT
                                      : INCLUDE_NONE );
                }

                if( lin ) {
                    new_val.set_flag( right_pos->is_right_included( )
                                      ? INCLUDE_RIGTH
                                      : INCLUDE_NONE );
                } else {
                    new_val.set_flag( p.is_right_included( )
                                      ? INCLUDE_RIGTH
                                      : INCLUDE_NONE );
                }

                if( res.second.isin ) {
                    ++right_pos;
                }

                trait_type::erase( cont, left_pos, right_pos );
                return trait_type::insert( cont, std::move(new_val) );
            }
        }

        iterator insert( contnr &cont, pos p )
        {
            auto res = locate<iterator>( cont, p );

            if( p.invalid( ) || p.empty( ) ) {
                return trait_type::end( cont );
            } else if( res.first.iter == trait_type::end( cont ) ) {
                return trait_type::insert( cont, std::move(p) );
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

                 trait_type::erase( cont, res.first.iter, res.second.iter );

                if( res.first.isin && !first.empty( ) ) {
                    trait_type::insert( cont, std::move(first) );
                }

                iterator ret = trait_type::insert( cont, p );

                if( res.second.isin && !last.empty( ) ) {
                    trait_type::insert( cont, std::move(last) );
                }

                return ret;
            }
        }

        iterator cut( contnr &cont, pos p )
        {
            auto res = locate<iterator>( cont, p );

            if( p.invalid( ) || p.is_both_excluded( ) ) {
                return trait_type::end( cont );
            } else if( res.first.iter == trait_type::end( cont ) ) {
                return trait_type::end( cont );
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

                iterator ret = trait_type::erase( cont,
                                                  res.first.iter,
                                                  res.second.iter );

                if( res.first.isin && !first.empty( ) ) {
                    trait_type::insert( cont, std::move(first) );
                }

                if( res.second.isin && !last.empty( ) ) {
                    ret = trait_type::insert( cont, std::move(last) );
                }

                return ret;
            }
        }
        contnr cont_;
    };

}}

#endif // SET_H
