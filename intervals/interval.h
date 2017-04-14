#ifndef ETOOL_INTERVALS_INTERVAL_H
#define ETOOL_INTERVALS_INTERVAL_H

#include <cstdint>
#include <string>
#include <sstream>

#include "etool/intervals/flags.h"

namespace etool { namespace intervals {

    template <typename ValueT>
    class interval {

        using value_type = ValueT;

    public:

        struct factory {

            using value_type    = interval::value_type;
            using interval_type = interval<value_type>;

            static
            interval_type infinite( )
            {
                return std::move(interval_type( value_type( ), value_type( ),
                                                INF_BOTH ) );
            }

            static
            interval_type open( value_type left, value_type right )
            {
                return std::move(interval_type( std::move(left),
                                                std::move(right),
                                                INCLUDE_NONE ) );
            }

            static
            interval_type closed( value_type left, value_type right )
            {
                return std::move(interval_type( std::move(left),
                                                std::move(right),
                                                INCLUDE_BOTH ) );
            }

            static
            interval_type degenerate( value_type left )
            {
                value_type right = left;
                return std::move(closed( std::move(left),
                                         std::move(right) ));
            }

            static
            interval_type left_open( value_type left )
            {
                return interval_type( std::move(left),
                                      value_type( ), INF_PLUS_RIGHT );
            }

            static
            interval_type left_open( value_type left, value_type right )
            {
                return interval_type( std::move(left), std::move(right),
                                      INCLUDE_RIGTH );
            }

            static
            interval_type right_open( value_type right )
            {
                return interval_type( value_type( ),
                                      std::move(right), INF_MINUS_LEFT );
            }

            static
            interval_type right_open( value_type left, value_type right )
            {
                return interval_type( std::move(left), std::move(right),
                                      INCLUDE_LEFT );
            }

            static
            interval_type left_closed( value_type left )
            {
                return interval_type( std::move(left), value_type( ),
                                      INF_PLUS_RIGHT | INCLUDE_LEFT );
            }

            static
            interval_type left_closed( value_type left, value_type right  )
            {
                return right_open( std::move(left), std::move(right) );
            }

            static
            interval_type right_closed( value_type right )
            {
                return interval<ValueT>( value_type( ), std::move(right),
                                         INF_MINUS_LEFT | INCLUDE_RIGTH );
            }

            static
            interval_type right_closed( value_type left, value_type right )
            {
                return left_open( std::move(left), std::move(right) );
            }
        };

        struct cmp: public std::binary_function<interval, interval, bool> {

            static
            value_type &min( value_type &lh, value_type &rh )
            {
                return less( lh, rh ) ? lh : rh;
            }

            static
            value_type &max( value_type &lh, value_type &rh )
            {
                return greater( lh, rh ) ? lh : rh;
            }

            static
            bool less( const value_type &lh, const value_type &rh )
            {
//                static const std::less<value_type> lesser;
//                return lesser(lh, rh);
                return lh < rh;
            }

            static
            bool less_equal( const value_type &lh, const value_type &rh )
            {
                return cmp::less( lh, rh ) || cmp::equal( lh, rh );
            }

            static
            bool greater( const value_type &lh, const value_type &rh )
            {
                return cmp::less( rh, lh ); // viceversa
            }

            static
            bool greater_equa( const value_type &lh, const value_type &rh )
            {
                return cmp::greater( lh, rh ) || equal( lh, rh );
            }

            static
            bool equal( const value_type &lh, const value_type &rh )
            {
                return !cmp::less( lh, rh )
                    && !cmp::less( rh, lh );
            }

            static
            bool less( const interval &lh, const interval &rh )
            {
                if( lh.is_right_inf( ) || rh.is_left_inf( ) ) {
                    return false;
                } else if( lh.is_right_close( ) && rh.is_left_close( ) ) {
                    return interval::cmp::less( lh.right( ), rh.left( ) );
                } else {
                    return interval::cmp::less_equal( lh.right( ), rh.left( ) );
                }
            }

            bool operator ( )( const interval &lh, const interval &rh ) const
            {
                return cmp::less( lh, rh );
            }

            bool operator ( )( const value_type &lh,
                               const value_type &rh ) const
            {
                return cmp::less( lh, rh );
            }
        };

    public:

        interval             ( )                    = default;
        interval             ( const interval & )   = default;
        interval &operator = ( const interval & )   = default;
        interval             ( interval && )        = default;
        interval& operator = ( interval && )        = default;

#if 1
        interval( value_type b, value_type e )
            :begin(b)
            ,end(e)
            ,flags(INCLUDE_LEFT)
        { }

        interval( value_type b, value_type e, std::uint32_t f )
            :begin(b)
            ,end(e)
            ,flags(f)
        { }

        bool valid( ) const
        {
            return has_inf( ) || cmp::less_equal( left( ), right( ) );
        }

#else
        interval( value_type b, value_type e )
            :begin(cmp::min(b, e))
            ,end(cmp::max(b, e))
            ,flags(INCLUDE_LEFT)
        { }

        interval( value_type b, value_type e, std::uint32_t f )
            :begin(cmp::min(b, e))
            ,end(cmp::max(b, e))
            ,flags(f)
        { }

        constexpr
        bool valid( ) const noexcept
        {
            return true;
        }
#endif
        bool has_inf( ) const noexcept
        {
            return (flags & (INF_MINUS_LEFT | INF_PLUS_RIGHT) ) != 0;
        }

        std::uint32_t right_flags( ) const noexcept
        {
            return (flags & (INCLUDE_RIGTH | INF_PLUS_RIGHT) );
        }

        bool is_right_close( ) const noexcept
        {
            return right_flags( ) == INCLUDE_RIGTH;
        }

        bool is_right_inf( ) const noexcept
        {
            return (flags & INF_PLUS_RIGHT) != 0;
        }

        std::uint32_t left_flags( ) const noexcept
        {
            return (flags & (INCLUDE_LEFT | INF_MINUS_LEFT));
        }

        bool is_left_close( ) const noexcept
        {
            return left_flags( ) == INCLUDE_LEFT;
        }

        bool is_left_inf( ) const noexcept
        {
            return (flags & INF_MINUS_LEFT) != 0;
        }

        bool is_both_close( ) const noexcept
        {
            return (flags & INCLUDE_BOTH) == INCLUDE_BOTH;
        }

        bool is_both_open( ) const noexcept
        {
            return flags == INCLUDE_NONE;
        }

        void set_flags( std::uint32_t f ) noexcept
        {
            flags = f;
        }

        void set_flag( std::uint32_t f ) noexcept
        {
            flags |= f;
        }

        void reset_flag( std::uint32_t f ) noexcept
        {
            flags &= (~f);
        }

        bool left_neighbor( const interval &ol ) const noexcept
        {
            return cmp::equal( left( ), ol.right( ) )
                && (left_flags( ) + ol.right_flags( ) > 0) ;
        }

        bool right_neighbor( const interval &ol ) const noexcept
        {
            return ol.left_neighbor( *this );
        }

        bool contain( const value_type &k ) const noexcept
        {
            bool bleft = is_left_inf( )
                    || ( is_left_close( )
                       ? cmp::greater_equa( k, begin )
                       : cmp::greater( k, begin ) );

            if( bleft ) {
                bool bright = is_right_inf( )
                         || ( is_right_close( )
                            ? cmp::less_equal( k, end )
                            : cmp::less( k, end ) );
                return bright;
            }
            return false;
        }

        bool contain_left( const interval &ol ) const noexcept
        {
            return ol.is_left_inf( ) || contain( ol.left( ) );
        }

        bool contain_right( const interval &ol ) const noexcept
        {
            return ol.is_right_inf( ) || contain( ol.right( ) );
        }

        const value_type &right( ) const
        {
            return end;
        }

        const value_type &left( ) const
        {
            return begin;
        }

        std::ostream &out( std::ostream &oss ) const
        {
            static const char lbracket[2] = { '(', '[' };
            static const char rbracket[2] = { ')', ']' };

            static const char *minf = "-inf";
            static const char *pinf = "+inf";

            oss << lbracket[is_left_close( )];

            if( left_flags( ) & INF_MINUS_LEFT ) {
                oss << minf;
            } else if( left_flags( ) & INF_PLUS_LEFT ) {
                oss << pinf;
            } else {
                oss << left( );
            }

            oss << ", ";

            if( right_flags( ) & INF_MINUS_RIGHT ) {
                oss << minf;
            } else if( right_flags( ) & INF_PLUS_RIGHT ) {
                oss << pinf;
            } else {
                oss << right( );
            }

            oss << rbracket[is_right_close( )];
            return oss;
        }

        std::string to_string(  ) const
        {
            std::ostringstream oss;
            out(oss);
            return oss.str( );
        }

        value_type length( ) const
        {
            return right( ) - left( );
        }

        bool invalid( ) const noexcept
        {
            return !valid( );
        }

        bool empty( ) const noexcept
        {
            return !has_inf( )
                && (cmp::equal( right( ), left( ) ) && flags != INCLUDE_BOTH);
        }

        bool intersected( const interval &other ) const noexcept
        {
            return intersected(*this, other);
        }

        static
        bool intersected( const interval &lh, const interval &rh ) noexcept
        {
            cmp c;
            return !c(lh, rh) && !c(rh, lh);
        }

    private:

        value_type      begin;
        value_type      end;
        std::uint32_t   flags = INCLUDE_LEFT;
    };

}}

#endif // INTERVAL_H
