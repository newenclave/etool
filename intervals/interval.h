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

        bool contain( const value_type &k ) const
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

        value_type &right( )
        {
            return end;
        }

        value_type &left( )
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

        bool intersect( const interval &other ) const
        {
            cmp c;
            return !c(*this, other) && !c(other, *this);
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
        };

    private:

        value_type      begin;
        value_type      end;
        std::uint32_t   flags = INCLUDE_LEFT;
    };

}}

#endif // INTERVAL_H
