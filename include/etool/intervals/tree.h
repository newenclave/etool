#ifndef ETOOL_INTERVALS_TREE_H
#define ETOOL_INTERVALS_TREE_H

#include "etool/intervals/interval.h"

namespace etool { namespace intervals {

    template <typename TraitT>
    class tree {
    public:

        using trait_type        = TraitT;

        using key_type          = typename trait_type::interval_type;
        using value_type        = typename trait_type::value_type;
        using container_type    = typename trait_type::container_type;

        using iterator          = typename trait_type::iterator;
        using const_iterator    = typename trait_type::const_iterator;

        using iterator_access   = typename trait_type::iterator_access;

        using data_type         = typename key_type::value_type;

        iterator begin( )
        {
            return cont_.begin( );
        }

        const_iterator begin( ) const
        {
            return cont_.begin( );
        }

        iterator end( )
        {
            return cont_.end( );
        }

        const_iterator end( ) const
        {
            return cont_.end( );
        }

        size_t size( ) const
        {
            return cont_.size( );
        }

        size_t empty( ) const
        {
            return cont_.empty( );
        }

        iterator erase( const_iterator itr )
        {
            cont_.erase(itr);
        }

        iterator erase( const_iterator b, const_iterator e )
        {
            cont_.erase( b, e );
        }

        iterator find( const data_type &key )
        {
            return find_impl(key);
        }

        const_iterator find( const data_type &key ) const
        {
            return find_const(key);
        }

        iterator find( const key_type &key )
        {
            using CT = container_type;
            auto res = locate<CT, iterator>(cont_, key);

            if(  res.left.itr == res.right.itr
              && res.left.contains
              && res.right.contains )
            {
                return res.left.itr;
            }

            return cont_.end( );
        }

        const_iterator find( const key_type &key ) const
        {
            using CCT = container_type;
            auto res = locate<CCT, iterator>(cont_, key);

            if(  res.left.itr == res.right.itr
              && res.left.contains
              && res.right.contains )
            {
                return res.left.itr;
            }

            return cont_.end( );
        }

    protected:

        tree( ) = default;

        template <typename ItrT>
        struct intersect_info {

            ItrT     itr;
            bool     contains;
            bool     connected;

            intersect_info( const intersect_info & ) = default;
            intersect_info& operator = ( const intersect_info & ) = default;
            intersect_info( intersect_info && ) = default;
            intersect_info& operator = ( intersect_info && ) = default;

            intersect_info( ItrT i, bool cont, bool conn )
                :itr(i)
                ,contains(cont)
                ,connected(conn)
            { }
        };

        template <typename ItrT>
        struct intersect_pair {
            intersect_info<ItrT> left;
            intersect_info<ItrT> right;

            intersect_pair( const intersect_pair & ) = default;
            intersect_pair& operator = ( const intersect_pair & ) = default;
            intersect_pair( intersect_pair && ) = default;
            intersect_pair& operator = ( intersect_pair && ) = default;

            intersect_pair( intersect_info<ItrT> l, intersect_info<ItrT> r )
                :left(std::move(l))
                ,right(std::move(r))
            { }
        };

        iterator insert_impl( value_type ival )
        {
#ifdef DEBUG
            if( !I::key(ival).valid( ) ) {
                throw std::logic_error( "Insert. Invalid value." );
                return cont_.end( );
            }
#endif

            using I = iterator_access;

            if( I::key(ival).is_infinite( ) ) {
                container_type tmp;
                tmp.insert( std::move(ival) );
                cont_.swap( tmp );
                return cont_.begin( );
            }
            auto pair = locate<container_type, iterator>( cont_, I::key(ival) );
            if( pair.left.itr == cont_.end( ) ) {
                return cont_.emplace_hint( cont_.end( ), std::move(ival) );
            }

            value_type first;
            value_type last;

            if( pair.left.contains ) {
                I::copy(first, I::val(pair.left.itr));
                I::mutable_key(first) =
                        I::key(pair.left.itr).connect_right( I::key(ival) );
            }

            if( pair.right.contains ) {
                I::copy(last, I::val(pair.right.itr));
                I::mutable_key(last) =
                        I::key(pair.right.itr).connect_left( I::key(ival) );
                pair.right.itr++;
            }

            auto tmp = cont_.erase( pair.left.itr, pair.right.itr );

            if( pair.left.contains && !I::key(first).empty( ) ) {
                tmp = cont_.emplace_hint( tmp, std::move(first) );
            }

            auto res = cont_.emplace_hint( tmp, std::move(ival) );

            if( pair.right.contains && !I::key(last).empty( ) ) {
                cont_.emplace_hint( res, std::move(last) );
            }

            return res;
        }

        iterator merge_impl( value_type ival )
        {
            using I  = iterator_access;
            using CT = container_type;

#ifdef DEBUG
            if( !I::key(ival).valid( ) ) {
                throw std::logic_error( "Merge. Invalid value." );
                return cont_.end( );
            }
#endif

            if( I::key(ival).is_infinite( ) ) {
                container_type tmp;
                tmp.insert( ival );
                cont_.swap( tmp );
                return cont_.begin( );
            }

            auto pair = locate<CT, iterator>( cont_, I::key(ival) );
            if( pair.left.itr == cont_.end( ) ) {
                return cont_.emplace_hint( cont_.end( ), std::move(ival) );
            }

            auto first_iter = pair.left.itr;
            if( pair.left.contains ) {
                I::mutable_key(ival).replace_left(I::key(first_iter));
                if( first_iter != cont_.begin( ) ) {
                    auto prev = std::prev(first_iter);
                    pair.left.connected =
                            I::key(ival).left_connected(I::key(prev));
                }
            }

            if( pair.left.connected ) {
                pair.left.itr--;
                first_iter = pair.left.itr;
                I::mutable_key(ival).replace_left(I::key(first_iter));
                while( pair.left.itr != cont_.begin( ) ) {
                    pair.left.itr--;
                    if( I::key(pair.left.itr).right_connected(
                                            I::key(first_iter) ) )
                    {
                        first_iter = pair.left.itr;
                        I::mutable_key(ival).replace_left(I::key(first_iter));
                    } else {
                        break;
                    }
                }
            }

            auto last_iter = pair.right.itr;
            if( pair.right.contains ) {
                I::mutable_key(ival).replace_right(I::key(last_iter));
                last_iter++;
                pair.right.connected = I::key(ival)
                                      .right_connected(I::key(last_iter));
            }
            if( pair.right.connected ) {
                last_iter = pair.right.itr;
                I::mutable_key(ival).replace_right(I::key(last_iter));
                ++pair.right.itr;
                while( pair.right.itr != cont_.end( ) ) {
                    bool c = I::key(pair.right.itr)
                            .left_connected( I::key(last_iter) );
                    if( c ) {
                        last_iter = pair.right.itr;
                        I::mutable_key(ival).replace_right(I::key(last_iter));
                    } else {
                        break;
                    }
                    ++pair.right.itr;
                }
                last_iter = pair.right.itr;
            }

            auto tmp = cont_.erase( first_iter, last_iter );
            return cont_.emplace_hint( tmp, std::move(ival) );

        }

        iterator cut_impl( const key_type &ival )
        {
            using I  = iterator_access;
            using CT = container_type;
#ifdef DEBUG
            if( !I::key(ival).valid( ) ) {
                throw std::logic_error( "Cut. Invalid value." );
                return cont_.end( );
            }
#endif

            if( ival.is_infinite( ) ) {
                container_type tmp;
                cont_.swap( tmp );
                return cont_.end( );
            }

            auto pair = locate<CT, iterator>( cont_, ival );
            if( pair.left.itr == cont_.end( ) ) {
                return cont_.end( );
            }

            value_type first;
            value_type last;

            if( pair.left.contains ) {
                I::copy(first, I::val(pair.left.itr));
                I::mutable_key(first) =
                        I::key(pair.left.itr).connect_right( ival );
            }

            if( pair.right.contains ) {
                I::copy(last, I::val(pair.right.itr));
                I::mutable_key(last) =
                        I::key(pair.right.itr).connect_left( ival );
                pair.right.itr++;
            }

            auto tmp = cont_.erase( pair.left.itr, pair.right.itr );

            if( pair.left.contains && !I::key(first).empty( ) ) {
                tmp = cont_.emplace_hint( tmp,  std::move(first) );
            }

            if( pair.right.contains && !I::key(last).empty( ) ) {
                tmp = cont_.emplace_hint( tmp, std::move(last) );
            }
            return tmp;
        }

    protected:

        template <typename Cont, typename ItrT = const_iterator>
        static
        intersect_pair<ItrT> locate( Cont &cont, const key_type &ival )
        {
#ifdef DEBUG
            if( !ival.valid( ) ) {
                throw std::logic_error( "Locate. Invalid value." );
                return cont_.end( );
            }
#endif
            using I = iterator_access;

            using res_type  = intersect_pair<ItrT>;
            using info_type = intersect_info<ItrT>;

            ItrT left = cont.lower_bound( ival );
            if( left == cont.end( ) ) {
                bool border = false;
                if( left != cont.begin( ) ) {
                    auto prev = std::prev( left );
                    border = ival.left_connected( I::key(prev) );
                }

                return res_type( info_type( cont.end( ), false, border ),
                                 info_type( cont.end( ), false, false  ) );

            }

            bool left_border   = false;
            bool left_contain  = I::key(left).contains_left( ival );

            if( left != cont.begin( ) ) {
                auto prev = std::prev(left);
                left_border = ival.left_connected( I::key(prev) );
            }

            bool right_border  = false;
            bool right_contain = false;

            ItrT right = cont.upper_bound( ival );

            if( right != cont.end( ) ) {
                right_border = ival.right_connected( I::key(right) );
            }

            if( right != cont.begin( ) ) {
                auto prev = std::prev( right );
                right_contain = I::key(prev).contains_right( ival );
                if( right_contain ) {
                    right = prev;
                }
            }

            return res_type( info_type(left,  left_contain,  left_border),
                             info_type(right, right_contain, right_border) );

        }

        iterator find_impl( const data_type &key )
        {
            using I  = iterator_access;
            using CT = container_type;

            auto res = locate<CT, iterator>( cont_, key );

            return I::key(res.left.itr).contains( key )
                 ? res.left.itr
                 : cont_.end( );
        }

        const_iterator find_const( const data_type &key ) const
        {
            using I   = iterator_access;
            using CCT = const container_type;
            auto res  = locate<CCT, const_iterator>( cont_, key );

            return I::key(res.left.itr).contains(key)
                 ? res.left.itr
                 : cont_.end( );
        }

    private:

        container_type cont_;
    };

}}

#endif // TREE_H
