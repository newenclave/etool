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

        interval( )                          = default;
        interval( const interval & )         = default;
        interval( interval && )              = default;
        interval& operator = ( interval && ) = default;

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
            return cmp::less_equal( left( ), right( ) );
        }

#else
        interval( value_type b, value_type e )
            :begin(std::min(b, e))
            ,end(std::max(b, e))
            ,flags(INCLUDE_LEFT)
        { }

        interval( value_type b, value_type e, std::uint32_t f )
            :begin(std::min(b, e))
            ,end(std::max(b, e))
            ,flags(f)
        { }

        constexpr
        bool valid( ) const noexcept
        {
            return true;
        }
#endif
        std::uint32_t right_flag( ) const noexcept
        {
            return (flags & INCLUDE_RIGTH);
        }

        bool is_right_included( ) const noexcept
        {
            return right_flag( ) != 0;
        }

        std::uint32_t left_flag( ) const noexcept
        {
            return (flags & INCLUDE_LEFT);
        }

        bool is_left_included( ) const noexcept
        {
            return left_flag( ) != 0;
        }

        bool is_both_included( ) const noexcept
        {
            return (flags & INCLUDE_BOTH) == INCLUDE_BOTH;
        }

        bool is_both_excluded( ) const noexcept
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

        bool left_neighbor( const interval &ol )
        {
            return cmp::equal( left( ), ol.right( ) )
                && (left_flag( ) + ol.right( ) > 0) ;
        }

        bool right_neighbor( const interval &ol )
        {
            return ol.left_neighbor( *this );
        }

        bool contain( const value_type &k ) const noexcept
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

        value_type &right( ) noexcept
        {
            return end;
        }

        value_type &left( ) noexcept
        {
            return begin;
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
            return cmp::equal( right( ), left( ) ) &&
                   flags != INCLUDE_BOTH;
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
            bool less( const value_type &lh, const value_type &rh )
            {
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

            bool operator ( )( const interval &lh, const interval &rh ) const
            {
                if( lh.is_right_included( ) && rh.is_left_included( ) ) {
                    return interval::cmp::less( lh.right( ), rh.left( ) );
                } else {
                    return interval::cmp::less_equal( lh.right( ), rh.left( ) );
                }
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

}}

#endif // INTERVAL_H
