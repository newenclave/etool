#ifndef ETOOL_INTERVALS_SET_H
#define ETOOL_INTERVALS_SET_H

#include <cstdint>
#include <string>
#include <sstream>

#include "etool/intervals/flags.h"
#include "etool/intervals/interval.h"
#include "etool/intervals/traits/std_set.h"
#include "etool/intervals/traits/vector_set.h"

#include "etool/intervals/common.h"

namespace etool { namespace intervals {

    template <typename KeyT,
              template <typename> class PosTraitT = traits::std_set>
    class set: public common<PosTraitT<KeyT> > {

    public:

        using key_type   = KeyT;
        using trait_type = PosTraitT<key_type>;

        using position = typename trait_type::position;

    private:

        using container  = typename trait_type::container;
        using super_type = common<trait_type>;

    public:

        using iterator       = typename trait_type::iterator;
        using const_iterator = typename trait_type::const_iterator;

        template<typename IterT>
        using container_slice = typename super_type::
                                template container_slice<IterT>;

        iterator merge( position p )
        {
            return merge( cont( ), p );
        }

        iterator merge( const key_type &lft, const key_type &rght )
        {
            return merge( cont( ), position(lft, rght) );
        }

        iterator merge( const key_type &uniq )
        {
            return merge( cont( ), position( uniq, uniq,
                                           intervals::INCLUDE_BOTH ) );
        }

        iterator merge( const key_type &lft, const key_type &rght,
                        std::uint32_t flags )
        {
            return merge( cont( ), position(lft, rght, flags) );
        }

        iterator insert( position p )
        {
            return insert( cont( ), p );
        }

        iterator insert( const key_type &lft, const key_type &rght )
        {
            return insert( cont( ), position(lft, rght) );
        }

        iterator insert( const key_type &uniq  )
        {
            return insert( cont( ), position( uniq, uniq,
                                            intervals::INCLUDE_BOTH) );
        }

        iterator insert( const key_type &lft, const key_type &rght,
                         std::uint32_t flags )
        {
            return insert( cont( ), position(lft, rght, flags) );
        }

        iterator cut( position p )
        {
            return cut( cont( ), p );
        }

        iterator cut( const key_type &lft, const key_type &rght )
        {
            return cut( cont( ), position(lft, rght) );
        }

        iterator cut( const key_type &uniq  )
        {
            return cut( cont( ), position( uniq, uniq,
                                           intervals::INCLUDE_BOTH) );
        }

        iterator cut( const key_type &lft, const key_type &rght,
                      std::uint32_t flags )
        {
            return cut( cont( ), position(lft, rght, flags) );
        }

        template <typename OutT>
        OutT &out( OutT &o ) const
        {
            return out( o, " " );
        }

        template <typename OutT, typename T>
        OutT &out( OutT &o, T gap ) const
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

    private:

        container &cont( )
        {
            return this->get_container( );
        }

        const container &cont( ) const
        {
            return this->get_container( );
        }

        using iter_access = typename trait_type::iterator_access;

        static
        iterator merge( container &cont, position p )
        {
            auto res = super_type::template locate<iterator>( cont, p );

            if( p.invalid( ) || p.empty( ) ) {
                return trait_type::end( cont );
            } else if( res.first.iter == trait_type::end( cont ) &&
                      !res.first.border ) {
                return trait_type::insert_hint( cont, res.first.iter,
                                                std::move(p) );
            } else {

                if( res.first.border ) {
                    std::advance(res.first.iter, -1);
                    res.first.inside = true;
                }

                if( res.second.border ) {
                    res.second.inside = true;
                }

                bool fin  = res.first.inside;
                bool lin  = res.second.inside;

                auto left_pos  = iter_access::get(res.first.iter);
                auto right_pos = iter_access::get(res.second.iter);

                position new_val( fin ? left_pos->left( )   : p.left( ),
                                  lin ? right_pos->right( ) : p.right( ),
                                  INCLUDE_NONE );

                if( fin ) {
                    new_val.set_flag( left_pos->left_flags( ) );
                } else {
                    new_val.set_flag( p.left_flags( ) );
                }

                if( lin ) {
                    new_val.set_flag( right_pos->right_flags( ) );
                } else {
                    new_val.set_flag( p.right_flags( ) );
                }

                if( res.second.inside ) {
                    std::advance(res.second.iter, +1);
                }

                auto t = trait_type::erase( cont, res.first.iter,
                                            res.second.iter );
                return trait_type::insert_hint( cont, t, std::move(new_val) );
            }
        }

        static
        iterator insert( container &cont, position p )
        {
            auto res = super_type::template locate<iterator>( cont, p );

            if( p.invalid( ) || p.empty( ) ) {
                return trait_type::end( cont );
            } else if( res.first.iter == trait_type::end( cont ) ) {
                return trait_type::insert_hint( cont, res.first.iter,
                                                std::move(p) );
            } else {

                position first;
                position last;

                if( res.first.inside ) {

                    std::uint32_t linc = res.first.iter->left_flags( );

                    std::uint32_t rinc = p.is_left_close( )
                                       ? INCLUDE_NONE
                                       : INCLUDE_RIGTH;

                    first = position( res.first.iter->left( ), p.left( ),
                                      linc | rinc );
                }

                if( res.second.inside ) {

                    std::uint32_t linc = p.is_right_close( )
                                       ? INCLUDE_NONE
                                       : INCLUDE_LEFT;

                    std::uint32_t rinc = res.second.iter->right_flags( );

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
            auto res = super_type::template locate<iterator>( cont, p );

            if( p.invalid( ) || p.is_both_open( ) ) {
                return trait_type::end( cont );
            } else if( res.first.iter == trait_type::end( cont ) ) {
                return trait_type::end( cont );
            } else {

                position first;
                position last;

                if( res.first.inside ) {

                    std::uint32_t linc = res.first.iter->left_flags( );

                    std::uint32_t rinc = p.is_left_close( )
                                       ? INCLUDE_NONE
                                       : INCLUDE_RIGTH;

                    first = position( res.first.iter->left( ), p.left( ),
                                      linc | rinc );
                }

                if( res.second.inside ) {

                    std::uint32_t linc = p.is_right_close( )
                                       ? INCLUDE_NONE
                                       : INCLUDE_LEFT;

                    std::uint32_t rinc = res.second.iter->right_flags( );

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
    };

}}

#endif // SET_H
