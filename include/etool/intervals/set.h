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

    template <typename KeyT, typename PosTraitT = traits::std_set<KeyT> >
    class set: public common<PosTraitT> {

    public:

        using key_type   = KeyT;
        using trait_type = PosTraitT;

        using position = typename trait_type::position;

        using factory  = typename position::factory;

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
            return merge( cont( ), factory::degenerate(uniq) );
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
            return insert( cont( ), factory::degenerate(uniq) );
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
            return cut( cont( ), factory::degenerate( uniq ) );
        }

        iterator cut( const key_type &lft, const key_type &rght,
                      std::uint32_t flags )
        {
            return cut( cont( ), position(lft, rght, flags) );
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

        template <typename OutT>
        OutT &out( OutT &o ) const
        {
            return out( o, " " );
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
            if( p.is_max_inf( ) ) {
                trait_type::clear( cont );
                trait_type::insert_hint( cont, trait_type::begin(cont),
                                         std::move(p) );
                return trait_type::begin(cont);
            }

            auto res = super_type::template locate<iterator>( cont, p );

            if( p.is_minus_inf( ) &&
                iter_access::get(res.first.iter)->is_left_inf( ) )
            {
                return res.first.iter;
            }

            if( p.is_plus_inf( ) && res.second.inside ) {
                auto prev = std::prev(res.second.iter);
                if(iter_access::get(prev)->is_right_inf( )) {
                    return res.second.iter;
                }
            }

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

                position new_val =
                      factory::closed( fin ? left_pos->left( )   : p.left( ),
                                       lin ? right_pos->right( ) : p.right( ));

                if( fin ) {
                    new_val.set_left_flag( left_pos->left_flags( ) );
                } else {
                    new_val.set_left_flag( p.left_flags( ) );
                }

                if( lin ) {
                    new_val.set_right_flag( right_pos->right_flags( ) );
                } else {
                    new_val.set_right_flag( p.right_flags( ) );
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
            if( p.is_max_inf( ) ) {
                trait_type::clear( cont );
                trait_type::insert_hint( cont, trait_type::begin(cont),
                                         std::move(p) );
                return trait_type::begin(cont);
            }

            auto res = super_type::template locate<iterator>( cont, p );

            if( p.is_minus_inf( ) &&
                iter_access::get(res.first.iter)->is_left_inf( ) )
            {
                return res.first.iter;
            }

            if( p.is_plus_inf( ) && res.second.inside ) {
                auto prev = std::prev(res.second.iter);
                if(iter_access::get(prev)->is_right_inf( )) {
                    return res.second.iter;
                }
            }

            if( p.invalid( ) || p.empty( ) ) {
                return trait_type::end( cont );
            } else if( res.first.iter == trait_type::end( cont ) ) {
                return trait_type::insert_hint( cont, res.first.iter,
                                                std::move(p) );
            } else {

                auto first_last = super_type::template
                                  make_first_last<iterator>( res, p );

                auto t = trait_type::erase( cont, res.first.iter,
                                             res.second.iter );

                if( res.first.inside && !first_last.first.empty( ) ) {
                    t = trait_type::insert_hint( cont, t,
                                              std::move(first_last.first) );
                }

                iterator ret = trait_type::insert_hint( cont, t, p );

                if( res.second.inside && !first_last.second.empty( ) ) {
                    trait_type::insert_hint( cont, ret,
                                           std::move(first_last.second) );
                }
                return ret;

            }
        }

        static
        iterator cut( container &cont, position p )
        {
            if( p.is_max_inf( ) ) {
                trait_type::clear( cont );
                return trait_type::begin(cont);
            }

            if( p.is_minus_inf( ) ) {
                return trait_type::begin(cont);
            }

            if( p.is_plus_inf( ) ) {
                return trait_type::end( cont );
            }

            auto res = super_type::template locate<iterator>( cont, p );

            if( p.invalid( ) || p.is_both_open( ) ) {
                return trait_type::end( cont );
            } else if( res.first.iter == trait_type::end( cont ) ) {
                return trait_type::end( cont );
            } else {

                auto first_last = super_type::template
                                  make_first_last<iterator>( res, p );

                iterator ret = trait_type::erase( cont,
                                                  res.first.iter,
                                                  res.second.iter );
                auto t = ret;

                if( res.first.inside &&
                   !first_last.first.empty( ) &&
                   !p.is_left_inf( ) )
                {
                    t = trait_type::insert_hint( cont, t,
                                                 std::move(first_last.first) );
                }

                if( res.second.inside &&
                   !first_last.second.empty( ) &&
                   !p.is_right_inf( ))
                {
                    ret = trait_type::insert_hint( cont, t,
                                                std::move(first_last.second) );
                }

                return ret;
            }
        }
    };

    template <typename KeyT, typename TraitT>
    inline
    std::ostream & operator << ( std::ostream &o,
                                 const set<KeyT, TraitT> &val )
    {
        return val.out(o);
    }


}}

#endif // SET_H
