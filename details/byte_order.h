#ifndef ETOOL_BYTE_ORDER_H
#define ETOOL_BYTE_ORDER_H

#include <cstdint>
#include <climits>
#include <stdint.h>

#include "etool/details/host_byte_order.h"
#include "etool/details/byte_swap.h"

namespace etool { namespace details {

    template <typename T, endian>
    struct byte_order;

    ///////////////////////// u8
    template <endian E>
    struct byte_order<std::uint8_t, E> {
        typedef       std::uint8_t  value_type;

        value_type value( value_type v )
        {
            return v;
        }

        template <typename U>
        static
        size_t write( value_type v, U *out )
        {
            *reinterpret_cast<value_type *>(out) = v;
            return sizeof(value_type);
        }

        template <typename U>
        static
        value_type read( const U *out )
        {
            return *reinterpret_cast<const value_type *>(out);
        }
    };

#if 0 // {

    template <>
    struct byte_order<std::uint8_t, endian::LITTLE> {

        typedef       std::uint8_t  value_type;

        value_type value( value_type v )
        {
            return v;
        }

        template <typename U>
        static
        size_t write( value_type v, U *out )
        {
            *reinterpret_cast<value_type *>(out) = v;
            return sizeof(value_type);
        }

        template <typename U>
        static
        value_type read( const U *out )
        {
            return *reinterpret_cast<const value_type *>(out);
        }
    };

#endif // }

    ///////////////////////// u16
    template <>
    struct byte_order<std::uint16_t, endian::BIG> {

        typedef std::uint16_t value_type;

        static
        value_type value( value_type v )
        {
            return etool::details::host_byte_order::is_big_endian( )
                 ? v
                 : byte_swap<value_type>::swap( v );
        }

        template <typename U>
        static
        size_t write( value_type v, U *out )
        {
            std::uint8_t *o = reinterpret_cast<std::uint8_t *>(out);
            o[0] = ( v >>  8 ) & 0xFF;
            o[1] = ( v       ) & 0xFF;
            return sizeof(v);
        }

        template <typename U>
        static
        value_type read( const U *in )
        {
            const std::uint8_t *i = reinterpret_cast<const std::uint8_t *>(in);
            return ( static_cast<value_type>(i[0]) <<  8 ) |
                   ( static_cast<value_type>(i[1])       ) ;
        }
    };

    template <>
    struct byte_order<std::uint16_t, endian::LITTLE> {

        typedef std::uint16_t value_type;

        static
        value_type value( value_type v )
        {
            return etool::details::host_byte_order::is_little_endian( )
                 ? v
                 : byte_swap<value_type>::swap( v );
        }

        template <typename U>
        static
        size_t write( value_type v, U *out )
        {
            std::uint8_t *o = reinterpret_cast<std::uint8_t *>(out);
            o[1] = ( v >>  8 ) & 0xFF;
            o[0] = ( v       ) & 0xFF;
            return sizeof(v);
        }

        template <typename U>
        static
        value_type read( const U *in )
        {
            const std::uint8_t *i = reinterpret_cast<const std::uint8_t *>(in);
            return ( static_cast<value_type>(i[1]) <<  8 ) |
                   ( static_cast<value_type>(i[0])       ) ;
        }
    };

    ///////////////////////// u32
    template <>
    struct byte_order<std::uint32_t, endian::BIG> {
        typedef       std::uint32_t  value_type;

        static
        value_type value( value_type v )
        {
            return etool::details::host_byte_order::is_big_endian( )
                 ? v
                 : byte_swap<value_type>::swap( v );
        }

        template <typename U>
        static
        size_t write( value_type v, U *out )
        {
            std::uint8_t *o = reinterpret_cast<std::uint8_t *>(out);
            o[0] = ( v >> 24 ) & 0xFF;
            o[1] = ( v >> 16 ) & 0xFF;
            o[2] = ( v >>  8 ) & 0xFF;
            o[3] = ( v       ) & 0xFF;
            return sizeof(v);
        }

        template <typename U>
        static
        value_type read( const U *in )
        {
            const std::uint8_t *i = reinterpret_cast<const std::uint8_t *>(in);
            return ( static_cast<value_type>(i[0]) << 24 ) |
                   ( static_cast<value_type>(i[1]) << 16 ) |
                   ( static_cast<value_type>(i[2]) <<  8 ) |
                   ( static_cast<value_type>(i[3])       ) ;
        }
    };

    template <>
    struct byte_order<std::uint32_t, endian::LITTLE> {

        typedef       std::uint32_t  value_type;

        static
        value_type value( value_type v )
        {
            return etool::details::host_byte_order::is_little_endian( )
                 ? v
                 : byte_swap<value_type>::swap( v );
        }

        template <typename U>
        static
        size_t write( value_type v, U *out )
        {
            std::uint8_t *o = reinterpret_cast<std::uint8_t *>(out);
            o[3] = ( v >> 24 ) & 0xFF;
            o[2] = ( v >> 16 ) & 0xFF;
            o[1] = ( v >>  8 ) & 0xFF;
            o[0] = ( v       ) & 0xFF;
            return sizeof(v);
        }

        template <typename U>
        static
        value_type read( const U *in )
        {
            const std::uint8_t *i = reinterpret_cast<const std::uint8_t *>(in);
            return  ( static_cast<value_type>(i[3]) << 24 ) |
                    ( static_cast<value_type>(i[2]) << 16 ) |
                    ( static_cast<value_type>(i[1]) <<  8 ) |
                    ( static_cast<value_type>(i[0])       ) ;
        }
    };


    ///////////////////////// u64
    template <>
    struct byte_order<std::uint64_t, endian::BIG> {

        typedef       std::uint64_t value_type;

        static
        value_type value( value_type v )
        {
            return etool::details::host_byte_order::is_big_endian( )
                 ? v
                 : byte_swap<value_type>::swap( v );
        }

        template <typename U>
        static
        size_t write( value_type v, U *out )
        {
            std::uint8_t *o = reinterpret_cast<std::uint8_t *>(out);
            o[0] = ( v >> 56 ) & 0xFF;
            o[1] = ( v >> 48 ) & 0xFF;
            o[2] = ( v >> 40 ) & 0xFF;
            o[3] = ( v >> 32 ) & 0xFF;
            o[4] = ( v >> 24 ) & 0xFF;
            o[5] = ( v >> 16 ) & 0xFF;
            o[6] = ( v >>  8 ) & 0xFF;
            o[7] = ( v       ) & 0xFF;
            return sizeof(v);
        }

        template <typename U>
        static
        value_type read( const U *in )
        {
            const std::uint8_t *i = reinterpret_cast<const std::uint8_t *>(in);
            return  ( static_cast<value_type>(i[0]) << 56 ) |
                    ( static_cast<value_type>(i[1]) << 48 ) |
                    ( static_cast<value_type>(i[2]) << 40 ) |
                    ( static_cast<value_type>(i[3]) << 32 ) |
                    ( static_cast<value_type>(i[4]) << 24 ) |
                    ( static_cast<value_type>(i[5]) << 16 ) |
                    ( static_cast<value_type>(i[6]) <<  8 ) |
                    ( static_cast<value_type>(i[7])       ) ;
        }
    };

    template <>
    struct byte_order<std::uint64_t, endian::LITTLE> {

        typedef       std::uint64_t  value_type;

        static
        value_type value( value_type v )
        {
            return etool::details::host_byte_order::is_big_endian( )
                    ? v
                    : byte_swap<value_type>::swap( v );
        }

        template <typename U>
        static
        size_t write( value_type v, U *out )
        {
            std::uint8_t *o = reinterpret_cast<std::uint8_t *>(out);
            o[7] = ( v >> 56 ) & 0xFF;
            o[6] = ( v >> 48 ) & 0xFF;
            o[5] = ( v >> 40 ) & 0xFF;
            o[4] = ( v >> 32 ) & 0xFF;
            o[3] = ( v >> 24 ) & 0xFF;
            o[2] = ( v >> 16 ) & 0xFF;
            o[1] = ( v >>  8 ) & 0xFF;
            o[0] = ( v       ) & 0xFF;
            return sizeof(v);
        }

        template <typename U>
        static
        value_type read( const U *in )
        {
            const std::uint8_t *i = reinterpret_cast<const std::uint8_t *>(in);
            return  ( static_cast<value_type>(i[7]) << 56 ) |
                    ( static_cast<value_type>(i[6]) << 48 ) |
                    ( static_cast<value_type>(i[5]) << 40 ) |
                    ( static_cast<value_type>(i[4]) << 32 ) |
                    ( static_cast<value_type>(i[3]) << 24 ) |
                    ( static_cast<value_type>(i[2]) << 16 ) |
                    ( static_cast<value_type>(i[1]) <<  8 ) |
                    ( static_cast<value_type>(i[0])       ) ;
        }
    };

    template <typename T>
    using byte_order_little = byte_order<T, endian::LITTLE>;

    template <typename T>
    using byte_order_big = byte_order<T, endian::BIG>;

}}

#endif // ETOOL_BYTE_ORDER_H
