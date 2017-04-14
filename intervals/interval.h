#ifndef ETOOL_INTERVALS_INTERVAL_H
#define ETOOL_INTERVALS_INTERVAL_H

#include <cstdint>
#include <string>
#include <sstream>

#include "etool/intervals/flags.h"

namespace etool { namespace intervals {

    template <typename ValueT>
    class interval {

    public:

        using value_type = ValueT;

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
            return (flags & (INF_LEFT | INF_RIGHT) ) != 0;
        }

        std::uint32_t right_flags( ) const noexcept
        {
            return (flags & (INCLUDE_RIGTH | INF_RIGHT) );
        }

        bool is_right_close( ) const noexcept
        {
            return right_flags( ) == INCLUDE_RIGTH;
        }

        bool is_right_inf( ) const noexcept
        {
            return (flags & INF_RIGHT) != 0;
        }

        std::uint32_t left_flags( ) const noexcept
        {
            return (flags & (INCLUDE_LEFT | INF_LEFT));
        }

        bool is_left_close( ) const noexcept
        {
            return left_flags( ) == INCLUDE_LEFT;
        }

        bool is_left_inf( ) const noexcept
        {
            return (flags & INF_LEFT) != 0;
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

            static const char *inf = "inf";

            oss << lbracket[is_left_close( )];

            if( is_left_inf( ) ) {
                oss << '-' << inf;
            } else {
                oss << left( );
            }

            oss << ", ";

            if( is_right_inf( ) ) {
                oss << '+' << inf;
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

    private:

        value_type      begin;
        value_type      end;
        std::uint32_t   flags = INCLUDE_LEFT;
    };


    template <typename ValueT>
    interval<ValueT> infinite( )
    {
        return std::move(interval<ValueT>( ValueT( ), ValueT( ), INF_BOTH ) );
    }

    template <typename ValueT>
    interval<ValueT> open( ValueT left, ValueT right )
    {
        return std::move(interval<ValueT>( std::move(left), std::move(right),
                                           INCLUDE_NONE ) );
    }

    template <typename ValueT>
    interval<ValueT> closed( ValueT left, ValueT right )
    {
        return std::move(interval<ValueT>( std::move(left), std::move(right),
                                           INCLUDE_BOTH ) );
    }

    template <typename ValueT>
    interval<ValueT> degenerate( ValueT left )
    {
        ValueT right = left;
        return std::move(closed( std::move(left), std::move(right) ));
    }

    template <typename ValueT>
    interval<ValueT> left_open( ValueT left )
    {
        return interval<ValueT>( std::move(left), ValueT( ), INF_RIGHT );
    }

    template <typename ValueT>
    interval<ValueT> left_open( ValueT left, ValueT right )
    {
        return interval<ValueT>( std::move(left), std::move(right),
                                 INCLUDE_RIGTH );
    }

    template <typename ValueT>
    interval<ValueT> right_open( ValueT right )
    {
        return interval<ValueT>( ValueT( ), std::move(right), INF_LEFT );
    }

    template <typename ValueT>
    interval<ValueT> right_open( ValueT left, ValueT right )
    {
        return interval<ValueT>( std::move(left), std::move(right),
                                 INCLUDE_LEFT );
    }

    template <typename ValueT>
    interval<ValueT> left_closed( ValueT left )
    {
        return interval<ValueT>( std::move(left), ValueT( ),
                                 INF_RIGHT | INCLUDE_LEFT );
    }

    template <typename ValueT>
    interval<ValueT> left_closed( ValueT left, ValueT right  )
    {
        return right_open( std::move(left), std::move(right) );
    }

    template <typename ValueT>
    interval<ValueT> right_closed( ValueT right )
    {
        return interval<ValueT>( ValueT( ), std::move(right),
                                 INF_LEFT | INCLUDE_RIGTH );
    }

    template <typename ValueT>
    interval<ValueT> right_closed( ValueT left, ValueT right )
    {
        return left_open( std::move(left), std::move(right) );
    }

}}

#endif // INTERVAL_H
