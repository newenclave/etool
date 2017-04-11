#ifndef ETOOL_INTERVALS_SET_H
#define ETOOL_INTERVALS_SET_H


#include <set>
#include <cstdint>
#include <string>
#include <sstream>

#include "etool/intervals/flags.h"

namespace etool { namespace intervals {

    template <typename T>
    class set {
    public:
        using key_type = T;

        class pos {

        public:

            pos( )                     = default;
            pos( const pos & )         = default;
            pos( pos && )              = default;
            pos& operator = ( pos && ) = default;

            pos( key_type b, key_type e )
                :begin(b)
                ,end(e)
                ,flags(INCLUDE_RIGTH)
            { }

            pos( key_type b, key_type e, std::uint32_t f )
                :begin(b)
                ,end(e)
                ,flags(f)
            { }

            bool is_right_included( ) const
            {
                return (flags & INCLUDE_RIGTH) != 0;
            }

            bool is_left_included( ) const
            {
                return (flags & INCLUDE_LEFT) != 0;
            }

            void set_flags( std::uint32_t f )
            {
                flags = f;
            }

            void set_flag( std::uint32_t f )
            {
                flags |= f;
            }

            void reset_flag( std::uint32_t f )
            {
                flags &= (~f);
            }

            bool isin( const key_type &k ) const
            {
                bool bleft = is_left_included( )
                            ? cmp::greater_equa( k, begin )
                            : cmp::greater( k,begin );

                if( bleft ) {
                    bool bright = is_right_included( )
                               ? cmp::less_equal( k, end )
                               : cmp::less( k, end );
                    return bright;
                }
                return false;
            }

            key_type &right( )
            {
                return end;
            }

            key_type &left( )
            {
                return begin;
            }

            const key_type &right( ) const
            {
                return end;
            }

            const key_type &left( ) const
            {
                return begin;
            }

            std::ostream &out( std::ostream &oss ) const
            {
                static const char lbracket[2] = {'(', '['};
                static const char rbracket[2] = {')', ']'};
                oss << lbracket[is_left_included( )]
                    << left( ) << ", " << right( )
                    << rbracket[is_right_included( )]
                       ;
                return oss;
            }

            std::string to_string(  ) const
            {
                std::ostringstream oss;
                out(oss);
                return std::move(oss.str( ));
            }

            key_type length( ) const
            {
                return right( ) - left( );
            }

            bool valid( ) const
            {
                return cmp::less_equal( left( ), right( ) );
            }

            bool invalid( ) const
            {
                return !valid( );
            }

            bool empty( ) const
            {
                return cmp::equal( right( ), left( ) ) &&
                      flags != INCLUDE_BOTH;
            }

            struct cmp: public std::binary_function<pos, pos, bool> {

                static
                bool less( const key_type &lh, const key_type &rh )
                {
                    return lh < rh;
                }

                static
                bool less_equal( const key_type &lh, const key_type &rh )
                {
                    return less( lh, rh ) || equal( lh, rh );
                }

                static
                bool greater( const key_type &lh, const key_type &rh )
                {
                    return rh < lh;
                }

                static
                bool greater_equa( const key_type &lh, const key_type &rh )
                {
                    return greater( lh, rh ) || equal( lh, rh );
                }

                static
                bool equal( const key_type &lh, const key_type &rh )
                {
                    return !pos::cmp::less( lh, rh )
                        && !pos::cmp::less( rh, lh );
                }

                bool operator ( )( const pos &lh, const pos &rh ) const
                {
                    if( lh.is_right_included( ) && rh.is_left_included( ) ) {
                        return pos::cmp::less( lh.right( ), rh.left( ) );
                    } else {
                        return pos::cmp::less_equal( lh.right( ), rh.left( ) );
                    }
                }
            };

            friend struct cmp;

        private:

            key_type        begin;
            key_type        end;
            std::uint32_t   flags = INCLUDE_RIGTH;
        };

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

        iterator insert( pos p )
        {
            return insert( cont_, p );
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
            }

            if( res.first.iter == cont.end( ) ) {
                return cont.emplace( std::move(p) ).first;
            } else {

                bool fin = res.first.isin;
                bool lin = res.second.isin;

                auto &fpos = *res.first.iter;
                auto &lpos = *res.second.iter;

                pos new_val = pos( fin ? fpos.left( ) : p.left( ),
                                   lin ? lpos.right( ) : p.right( ),
                                   INCLUDE_NONE );

                if( fin ) {
                    new_val.set_flag( fpos.is_left_included( )
                                      ? INCLUDE_LEFT
                                      : INCLUDE_NONE );
                } else {
                    new_val.set_flag( p.is_left_included( )
                                      ? INCLUDE_LEFT
                                      : INCLUDE_NONE );
                }

                if( lin ) {
                    new_val.set_flag( lpos.is_right_included( )
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
            }

            if( res.first.iter == cont.end( ) ) {
                return cont.emplace( std::move(p) ).first;
            } else {

                pos first;
                pos last;


                if( res.first.isin ) {
                    //res.first.iter->out( std::cout ) << "\n";
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
                    //res.second.iter->out( std::cout ) << "\n";

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
