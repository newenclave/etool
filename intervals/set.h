#ifndef ETOOL_INTERVALS_SET_H
#define ETOOL_INTERVALS_SET_H

#include <cstdint>
#include <string>
#include <sstream>

#include "etool/intervals/flags.h"
#include "etool/intervals/interval.h"
#include "etool/intervals/traits/std_set.h"
#include "etool/intervals/traits/vector_set.h"

#include "etool/intervals/operations.h"

namespace etool { namespace intervals {

    template <typename KeyT,
              template <typename> class PosTraitT = traits::std_set>
    class set {

    public:

        using key_type   = KeyT;
        using trait_type = PosTraitT<key_type>;

        using position = typename trait_type::position;

    private:

        using container = typename trait_type::container;

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

        iterator find( const key_type &k )
        {
            return find( cont_, k );
        }

        const_iterator find( const key_type &k ) const
        {
            return find( cont_, k );
        }

        iterator merge( position p )
        {
            return merge( cont_, p );
        }

        iterator merge( const key_type &lft, const key_type &rght )
        {
            return merge( cont_, position(lft, rght) );
        }

        iterator merge( const key_type &uniq )
        {
            return merge( cont_, position( uniq, uniq,
                                           intervals::INCLUDE_BOTH ) );
        }

        iterator merge( const key_type &lft, const key_type &rght,
                        std::uint32_t flags )
        {
            return merge( cont_, position(lft, rght, flags) );
        }

        iterator insert( position p )
        {
            return insert( cont_, p );
        }

        iterator insert( const key_type &lft, const key_type &rght )
        {
            return insert( cont_, position(lft, rght) );
        }

        iterator insert( const key_type &uniq  )
        {
            return insert( cont_, position( uniq, uniq,
                                            intervals::INCLUDE_BOTH) );
        }

        iterator insert( const key_type &lft, const key_type &rght,
                         std::uint32_t flags )
        {
            return insert( cont_, position(lft, rght, flags) );
        }

        iterator cut( position p )
        {
            return cut( cont_, p );
        }

        iterator cut( const key_type &lft, const key_type &rght )
        {
            return cut( cont_, position(lft, rght) );
        }

        iterator cut( const key_type &uniq  )
        {
            return cut( cont_, position(uniq, uniq, intervals::INCLUDE_BOTH) );
        }

        iterator cut( const key_type &lft, const key_type &rght,
                      std::uint32_t flags )
        {
            return cut( cont_, position(lft, rght, flags) );
        }

        std::ostream &out( std::ostream &o ) const
        {
            return out(o, " ");
        }

        template <typename T>
        std::ostream &out( std::ostream &o, T gap ) const
        {
            bool start = true;
            for( auto &n: *this ) {
                if( !start ) {
                    o << gap;
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

        using my_oper = operations<trait_type>;

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
        iterator find( container &cont, const key_type &k )
        {
            auto res = locate<iterator>( cont, position( k, k, INCLUDE_BOTH ) );
            return res.first.inside ? res.first.iter : trait_type::end(cont);
        }

        static
        const_iterator find( const container &cont, const key_type &k )
        {
            auto res = locate<const_iterator>( cont, position( k, k ) );
            return res.first.inside ? res.first.iter : trait_type::end(cont);
        }

        static
        iterator merge( container &cont, position p )
        {
            auto res = locate<iterator>( cont, p );

            if( p.invalid( ) || p.empty( ) ) {
                return trait_type::end( cont );
            } else if( res.first.iter == trait_type::end( cont ) &&
                      !res.first.border ) {
                return trait_type::insert_hint( cont, res.first.iter,
                                                std::move(p) );
            } else {

                if( res.first.border ) {
                    res.first.iter = std::prev(res.first.iter);
                    res.first.inside = true;
                }

                if( res.second.border ) {
                    res.second.iter = std::next(res.second.iter);
                    res.second.inside = true;
                }

                bool fin  = res.first.inside;
                bool lin  = res.second.inside;

                auto left_pos  = iter_access::get(res.first.iter);
                auto right_pos = iter_access::get(res.second.iter);

                position new_val = position( fin ? left_pos->left( )   : p.left( ),
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

                if( res.second.inside ) {
                    ++res.second.iter;
                }

                auto t = trait_type::erase( cont, res.first.iter,
                                            res.second.iter );
                return trait_type::insert_hint( cont, t, std::move(new_val) );
            }
        }

        static
        iterator insert( container &cont, position p )
        {
            auto res = locate<iterator>( cont, p );

            if( p.invalid( ) || p.empty( ) ) {
                return trait_type::end( cont );
            } else if( res.first.iter == trait_type::end( cont ) ) {
                return trait_type::insert_hint( cont, res.first.iter,
                                                std::move(p) );
            } else {

                position first;
                position last;

                if( res.first.inside ) {

                    std::uint32_t linc = res.first.iter->is_left_included( )
                                       ? INCLUDE_LEFT
                                       : INCLUDE_NONE;

                    std::uint32_t rinc = p.is_left_included( )
                                       ? INCLUDE_NONE
                                       : INCLUDE_RIGTH;

                    first = position( res.first.iter->left( ), p.left( ),
                                 linc | rinc );
                }

                if( res.second.inside ) {

                    std::uint32_t linc = p.is_right_included( )
                                       ? INCLUDE_NONE
                                       : INCLUDE_LEFT;

                    std::uint32_t rinc = res.second.iter->is_right_included( )
                                       ? INCLUDE_RIGTH
                                       : INCLUDE_NONE;

                    last = position( p.right( ), res.second.iter->right( ),
                                linc | rinc );

                    ++res.second.iter;
                }

                 auto t = trait_type::erase( cont, res.first.iter,
                                             res.second.iter );

                if( res.first.inside && !first.empty( ) ) {
                    t = trait_type::insert_hint( cont, t, std::move(first) );
                }

                iterator ret = trait_type::insert_hint( cont, t, p );

                if( res.second.inside && !last.empty( ) ) {
                    trait_type::insert_hint( cont, ret, std::move(last) );
                }

                return ret;
            }
        }

        static
        iterator cut( container &cont, position p )
        {
            auto res = locate<iterator>( cont, p );

            if( p.invalid( ) || p.is_both_excluded( ) ) {
                return trait_type::end( cont );
            } else if( res.first.iter == trait_type::end( cont ) ) {
                return trait_type::end( cont );
            } else {

                position first;
                position last;

                if( res.first.inside ) {

                    std::uint32_t linc = res.first.iter->is_left_included( )
                                       ? INCLUDE_LEFT
                                       : INCLUDE_NONE;

                    std::uint32_t rinc = p.is_left_included( )
                                       ? INCLUDE_NONE
                                       : INCLUDE_RIGTH;

                    first = position( res.first.iter->left( ), p.left( ),
                                      linc | rinc );
                }

                if( res.second.inside ) {

                    std::uint32_t linc = p.is_right_included( )
                                       ? INCLUDE_NONE
                                       : INCLUDE_LEFT;

                    std::uint32_t rinc = res.second.iter->is_right_included( )
                                       ? INCLUDE_RIGTH
                                       : INCLUDE_NONE;

                    last = position( p.right( ), res.second.iter->right( ),
                                     linc | rinc );

                    ++res.second.iter;
                }

                iterator ret = trait_type::erase( cont,
                                                  res.first.iter,
                                                  res.second.iter );
                auto t = ret;

                if( res.first.inside && !first.empty( ) ) {
                    t = trait_type::insert_hint( cont, t, std::move(first) );
                }

                if( res.second.inside && !last.empty( ) ) {
                    ret = trait_type::insert_hint( cont, t, std::move(last) );
                }

                return ret;
            }
        }

        container cont_;
    };

}}

#endif // SET_H
