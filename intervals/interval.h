#ifndef ETOOL_INTERVALS_INTERVAL_H
#define ETOOL_INTERVALS_INTERVAL_H

#include <cstdint>
#include <string>
#include <sstream>

#include "etool/intervals/side_info.h"

namespace etool { namespace intervals {

    template <typename ValueT>
    class interval {

        using value_type = ValueT;

        enum { LEFT_SIDE = 0, RIGHT_SIDE = 1 };

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
                using LS = left_type;
                using RS = right_type;

                if( lh.is_minus_inf<RS>( ) ) {
                    return !rh.is_minus_inf<LS>( );
                } else if( lh.is_plus_inf<RS>( ) || rh.is_minus_inf<LS>( ) ) {
                    return false;
                } else if( rh.is_plus_inf<LS>( ) ) {
                    return true;
                } else if( lh.is_open<RS>( ) || rh.is_open<LS>( ) ) {
                    return interval::cmp::less_equal( lh.value<RS>( ),
                                                      rh.value<LS>( ) );
                } else {
                    return interval::cmp::less( lh.value<RS>( ),
                                                rh.value<LS>( ) );
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

        template <side_name Side>
        class side_access {

            using info = side_info<Side>;

        public:

            side_access( const interval *parent )
                :parent_(parent)
            { }

            bool is_minus_inf( ) const noexcept
            {
                return parent_->template is_minus_inf<info>( );
            }

            bool is_plus_inf( ) const noexcept
            {
                return parent_->template is_plus_inf<info>( );
            }

            bool is_open( ) const noexcept
            {
                return parent_->template is_open<info>( );
            }

            bool is_close( ) const noexcept
            {
                return parent_->template is_close<info>( );
            }

            const value_type &value( ) const
            {
                return parent_->template value<info>( );
            }

            const std::uint16_t &flags( ) const
            {
                return parent_->template flags<info>( );
            }

        private:

            const interval *parent_;
        };

    public:

        using left_type  = side_info<side_name::LEFT>;
        using right_type = side_info<side_name::RIGHT>;

        interval             ( )                    = default;
        interval             ( const interval & )   = default;
        interval &operator = ( const interval & )   = default;
        interval             ( interval && )        = default;
        interval& operator = ( interval && )        = default;

#if 1
        interval( value_type b, value_type e )
        {
            sides_[LEFT_SIDE]      = std::move(b);
            sides_[RIGHT_SIDE]     = std::move(e);
            flags_.u.lr[LEFT_SIDE]  = SIDE_CLOSE;
            flags_.u.lr[RIGHT_SIDE] = SIDE_OPEN;
        }

        interval( value_type b, value_type e,
                 std::uint16_t lf, std::uint16_t rf)
            :flags_(lf, rf)
        {
            sides_[LEFT_SIDE]  = std::move(b);
            sides_[RIGHT_SIDE] = std::move(e);
        }

        bool valid( ) const
        {
            return has_inf( ) || cmp::less_equal( left( ), right( ) );
        }

#else
        interval( value_type b, value_type e )
        {
            if( cmp::less( b, e ) ) {
                sides_[LEFT_SIDE]  = std::move(b);
                sides_[RIGHT_SIDE] = std::move(e);
            } else {
                sides_[LEFT_SIDE]  = std::move(e);
                sides_[RIGHT_SIDE] = std::move(b);
            }

            flags.u.lr[LEFT_SIDE] = SIDE_CLOSE;
            flags.u.lr[RIGHT_SIDE] = SIDE_OPEN;
        }

        interval( value_type b, value_type e,
                 std::uint16_t lf, std::uint16_t rf)
            :flags(lf, rf)
        {
            if( cmp::less( b, e ) ) {
                sides_[LEFT_SIDE]  = std::move(b);
                sides_[RIGHT_SIDE] = std::move(e);
            } else {
                sides_[LEFT_SIDE]  = std::move(e);
                sides_[RIGHT_SIDE] = std::move(b);
            }
            flags.u.lr[LEFT_SIDE]  = lf;
            flags.u.lr[RIGHT_SIDE] = rf;
        }

        constexpr
        bool valid( ) const noexcept
        {
            return true;
        }
#endif

//        template <typename SideT>
//        side_info side( ) const
//        {
//            side_info( value<SideT>( ), flags<SideT>( ) );
//        }

        template <typename SideT>
        bool is_minus_inf( ) const noexcept
        {
            return flags_.u.lr[SideT::side] == SIDE_MIN_INF;
        }

        template <typename SideT>
        bool is_plus_inf( ) const noexcept
        {
            return flags<SideT>( ) == SIDE_MAX_INF;
        }

        template <typename SideT>
        bool is_open( ) const noexcept
        {
            return flags<SideT>( ) == SIDE_OPEN;
        }

        template <typename SideT>
        bool is_close( ) const noexcept
        {
            return flags<SideT>( ) == SIDE_CLOSE;
        }

        template <typename SideT>
        const value_type &value( ) const
        {
            return sides_[SideT::side];
        }

        template <typename SideT>
        const std::uint16_t &flags( ) const
        {
            return flags_.u.lr[SideT::side];
        }

        template <typename SideT>
        void set_flag( std::uint16_t f )
        {
            flags_.u.lr[SideT::side] = f;
        }

        void set_left_flag( std::uint16_t f )
        {
            set_flag<left_type>( f );
        }

        void set_right_flag( std::uint16_t f )
        {
            set_flag<right_type>( f );
        }

        bool has_inf( ) const noexcept
        {
            return (flags<left_type>( )  == SIDE_MIN_INF)
                || (flags<left_type>( )  == SIDE_MAX_INF)

                || (flags<right_type>( ) == SIDE_MIN_INF)
                || (flags<right_type>( ) == SIDE_MAX_INF)
                 ;
        }

        bool is_minus_inf( ) const noexcept
        {
            return (left_flags( )  == SIDE_MIN_INF)
                && (right_flags( ) == SIDE_MIN_INF)
                 ;
        }

        bool is_max_inf( ) const noexcept
        {
            return (left_flags( )  == SIDE_MIN_INF)
                && (right_flags( ) == SIDE_MAX_INF)
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
            return flags<right_type>( );
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
            return flags<left_type>( );
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
                       ? cmp::greater_equa( k, left( ) )
                       : cmp::greater( k, left( ) ) );

            if( bleft ) {
                bool bright = is_right_inf( )
                         || ( is_right_close( )
                            ? cmp::less_equal( k, right( ) )
                            : cmp::less( k, right( ) ) );
                return bright;
            }
            return false;
        }

        const value_type &right( ) const
        {
            return sides_[RIGHT_SIDE];
        }

        const value_type &left( ) const
        {
            return sides_[LEFT_SIDE];
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

        bool contains_left( const interval &rhi ) const
        {
            using LS = left_type;
            bool res = false;

            /// LEFT point is placed right
            if( is_minus_inf<LS>( ) ) {
                res = rhi.is_minus_inf<LS>( );
            } else if( is_plus_inf<LS>( ) ) {
                res = rhi.is_plus_inf<LS>( );
            }

            if( !res ) {
                res = contain( rhi.left( ) );
            }

            return res;
        }

        bool contains_right( const interval &rhi ) const
        {
            using RS = right_type;
            bool res = false;

            /// RIGHT point is placed right
            if( is_minus_inf<RS>( ) ) {
                res = rhi.is_minus_inf<RS>( );
            } else if( is_plus_inf<RS>( ) ) {
                res = rhi.is_plus_inf<RS>( );
            }

            if( !res ) {
                res = contain( rhi.right( ) );
            }

            return res;
        }

        static
        std::pair<bool, bool> intersections( const interval &lhi,
                                             const interval &rhi )
        {
            return std::make_pair( lhi.contains_left( rhi ),
                                   lhi.contains_right( rhi ) );
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

        value_type      sides_[2];
        flags_value     flags_;
    };

    template <typename ValueT>
    inline
    std::ostream & operator << ( std::ostream &o, const interval<ValueT> &val )
    {
        return val.out(o);
    }

}}

#endif // INTERVAL_H
