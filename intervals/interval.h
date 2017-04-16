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
                                                SIDE_MIN_INF, SIDE_MAX_INF ) );
            }

            static
            interval_type open( value_type left, value_type right )
            {
                return std::move(interval_type( std::move(left),
                                                std::move(right),
                                                SIDE_OPEN, SIDE_OPEN ) );
            }

            static
            interval_type closed( value_type left, value_type right )
            {
                return std::move(interval_type( std::move(left),
                                                std::move(right),
                                                SIDE_CLOSE, SIDE_CLOSE ) );
            }

            static
            interval_type degenerate( value_type left )
            {
                value_type right = left;
                return std::move( closed( std::move(left), std::move(right) ) );
            }

            static
            interval_type left_open( value_type left )
            {
                return interval_type( std::move(left), value_type( ),
                                      SIDE_OPEN, SIDE_MAX_INF );
            }

            static
            interval_type left_open( value_type left, value_type right )
            {
                return interval_type( std::move(left), std::move(right),
                                      SIDE_OPEN, SIDE_CLOSE );
            }

            static
            interval_type right_open( value_type right )
            {
                return interval_type( value_type( ),
                                      std::move(right),
                                      SIDE_MIN_INF, SIDE_OPEN );
            }

            static
            interval_type right_open( value_type left, value_type right )
            {
                return interval_type( std::move(left), std::move(right),
                                      SIDE_CLOSE, SIDE_OPEN );
            }

            static
            interval_type left_closed( value_type left )
            {
                return interval_type( std::move(left), value_type( ),
                                      SIDE_CLOSE, SIDE_MAX_INF );
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
                                         SIDE_MIN_INF, SIDE_CLOSE );
            }

            static
            interval_type right_closed( value_type left, value_type right )
            {
                return left_open( std::move(left), std::move(right) );
            }
        };

        class side_info {

        public:

            side_info( const value_type &val, std::uint32_t flags )
                :val_(val)
                ,flags_(flags)
            { }

            const value_type &value( ) const
            {
                return val_;
            }

            bool is_minus_inf( ) const
            {
                return flags_ == SIDE_MIN_INF;
            }

            bool is_plus_inf( ) const
            {
                return flags_ == SIDE_MAX_INF;
            }

            bool is_close( ) const
            {
                return flags_ == SIDE_CLOSE;
            }

            bool is_open( ) const
            {
                return flags_ == SIDE_OPEN;
            }

            static
            bool less( const side_info &lh, const side_info &rh )
            {
                if( lh.is_minus_inf( ) ) {
                    return !rh.is_minus_inf( );
                } else if( lh.is_plus_inf( ) || rh.is_minus_inf( ) ) {
                    return false;
                } else if( rh.is_plus_inf( ) ) {
                    return true;
                } else if( lh.is_open( ) ) {
                    return interval::cmp::less_equal( lh.value( ),
                                                      rh.value( ) );
                } else {
                    return interval::cmp::less( lh.value( ), rh.value( ) );
                }
            }

        private:
            const value_type &val_;
            std::uint32_t     flags_;
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
            bool less( const interval &lhi, const interval &rhi )
            {
                auto lh = lhi.right_side( );
                auto rh = rhi.left_side( );

                if( lh.is_minus_inf( ) ) {
                    return !rh.is_minus_inf( );
                } else if( lh.is_plus_inf( ) || rh.is_minus_inf( ) ) {
                    return false;
                } else if( rh.is_plus_inf( ) ) {
                    return true;
                } else if( lh.is_open( ) || rh.is_open( ) ) {
                    return interval::cmp::less_equal( lh.value( ),
                                                      rh.value( ) );
                } else {
                    return interval::cmp::less( lh.value( ), rh.value( ) );
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
            :left_(b)
            ,right_(e)
        {
            flags.u.lr[0] = SIDE_CLOSE;
            flags.u.lr[1] = SIDE_OPEN;
        }

        interval( value_type b, value_type e,
                 std::uint16_t lf, std::uint16_t rf)
            :left_(b)
            ,right_(e)
            ,flags(lf, rf)
        { }

        bool valid( ) const
        {
            return has_inf( ) || cmp::less_equal( left( ), right( ) );
        }

#else
        interval( value_type b, value_type e )
            :begin(cmp::min(b, e))
            ,end(cmp::max(b, e))
        {
            flags.u.lr[0] = SIDE_CLOSE;
            flags.u.lr[1] = SIDE_OPEN;
        }

        constexpr
        bool valid( ) const noexcept
        {
            return true;
        }
#endif

        side_info left_side( ) const
        {
            return side_info(left_, left_flags( ));
        }

        side_info right_side( ) const
        {
            return side_info(right_, right_flags( ));
        }

        bool has_inf( ) const noexcept
        {
            return (left_flags( )  == SIDE_MIN_INF)
                || (left_flags( )  == SIDE_MAX_INF)

                || (right_flags( ) == SIDE_MIN_INF)
                || (right_flags( ) == SIDE_MAX_INF)
                 ;
        }

        bool is_minus_inf( ) const noexcept
        {
            return (left_flags( )  == SIDE_MIN_INF)
                && (right_flags( ) == SIDE_MIN_INF)
                 ;
        }

        bool is_plus_inf( ) const noexcept
        {
            return (left_flags( )  == SIDE_MAX_INF)
                && (right_flags( ) == SIDE_MAX_INF)
                 ;
        }

        bool is_side_inf( ) const noexcept
        {
            return is_minus_inf( ) || is_plus_inf( );
        }

        std::uint16_t right_flags( ) const noexcept
        {
            return flags.u.lr[1];
        }

        bool is_right_close( ) const noexcept
        {
            return right_flags( ) == SIDE_CLOSE;
        }

        bool is_right_inf( ) const noexcept
        {
            return right_flags( ) == SIDE_MAX_INF;
        }

        std::uint32_t left_flags( ) const noexcept
        {
            return flags.u.lr[0];
        }

        bool is_left_close( ) const noexcept
        {
            return left_flags( ) == SIDE_CLOSE;
        }

        bool is_left_inf( ) const noexcept
        {
            return left_flags( ) == SIDE_MIN_INF;
        }

        bool is_both_close( ) const noexcept
        {
            return ( left_flags( )  == SIDE_CLOSE )
                && ( right_flags( ) == SIDE_CLOSE );
        }

        bool is_both_open( ) const noexcept
        {
            return ( left_flags( )  == SIDE_OPEN )
                && ( right_flags( ) == SIDE_OPEN );
        }

        void set_left( std::uint16_t f ) noexcept
        {
            flags.u.lr[0] = f;
        }

        void set_right( std::uint32_t f ) noexcept
        {
            flags.u.lr[1] = f;
        }

        bool left_connected( const interval &ol ) const noexcept
        {
            return cmp::equal( left( ), ol.right( ) )
                && (is_left_close( ) || ol.is_right_close( )) ;
        }

        bool right_connected( const interval &ol ) const noexcept
        {
            return ol.left_connected( *this );
        }

        bool contain( const value_type &k ) const noexcept
        {
            if( is_side_inf( ) ) {
                return false;
            }

            bool bleft = is_left_inf( )
                    || ( is_left_close( )
                       ? cmp::greater_equa( k, left_ )
                       : cmp::greater( k, left_ ) );

            if( bleft ) {
                bool bright = is_right_inf( )
                         || ( is_right_close( )
                            ? cmp::less_equal( k, right_ )
                            : cmp::less( k, right_ ) );
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
            return right_;
        }

        const value_type &left( ) const
        {
            return left_;
        }

        std::ostream &out( std::ostream &oss ) const
        {
            static const char lbracket[2] = { '(', '[' };
            static const char rbracket[2] = { ')', ']' };

            static const char *minf = "-inf";
            static const char *pinf = "+inf";

            oss << lbracket[is_left_close( )];

            if( left_flags( ) == SIDE_MIN_INF ) {
                oss << minf;
            } else if( left_flags( ) == SIDE_MAX_INF ) {
                oss << pinf;
            } else {
                oss << left( );
            }

            oss << ", ";

            if( right_flags( ) == SIDE_MIN_INF ) {
                oss << minf;
            } else if( right_flags( ) == SIDE_MAX_INF ) {
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
                && ( cmp::equal( right( ), left( ) )
                     && !is_both_close( ) );
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
        struct flags_value {

            union {
                std::uint32_t all;
                std::uint16_t lr[2] = { 0, 0};
            } u;

            flags_value( ) = default;
            flags_value( std::uint16_t l, std::uint16_t r )
            {
                u.lr[0] = l;
                u.lr[1] = r;
            }
        };

        value_type      left_;
        value_type      right_;
        flags_value     flags;
    };

    template <typename ValueT>
    inline
    std::ostream & operator << ( std::ostream &o, const interval<ValueT> &val )
    {
        return val.out(o);
    }

}}

#endif // INTERVAL_H
