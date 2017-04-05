#ifndef ETOOL_DETAILS_BYTE_SWAP_H
#define ETOOL_DETAILS_BYTE_SWAP_H

#include <cstdint>

namespace etool { namespace details {

    template <typename T>
    struct byte_swap;

    template <typename T>
    struct byte_swap {

        typedef T value_type;

        static
        void place_swap( value_type &v )
        {
            std::uint8_t *in  = reinterpret_cast<std::uint8_t *>(&v);
            static const size_t length = sizeof(value_type);
            static const size_t middle = length / 2;

            for( size_t i=0; i < middle; ++i ) {
                std::uint8_t tmp    = in[length - 1 - i];
                in[length - 1 - i]  = in[i];
                in[i]               = tmp;
            }
        }

        static
        value_type swap( value_type v )
        {
            place_swap( v );
            return v;
        }
    };

    template <>
    struct byte_swap<std::uint8_t> {

        typedef std::uint8_t value_type;

        static
        void place_swap( value_type & )
        { }

        static
        value_type swap( value_type v )
        {
            return v;
        }
    };

    template <>
    struct byte_swap<std::uint16_t> {
        typedef std::uint16_t value_type;

        static
        void place_swap( value_type &v )
        {
            v = swap( v );
        }

        static
        value_type swap( value_type v )
        {
            return ( ( v & 0xFF00 ) >> 8 ) |
                   ( ( v & 0x00FF ) << 8 ) ;
        }
    };

    template <>
    struct byte_swap<std::uint32_t> {
        typedef std::uint32_t value_type;

        static
        void place_swap( value_type &v )
        {
            v = swap( v );
        }

        static
        value_type swap( value_type v )
        {
            return ( ( v & 0xFF000000 ) >> 24 ) |
                   ( ( v & 0x00FF0000 ) >>  8 ) |
                   ( ( v & 0x0000FF00 ) <<  8 ) |
                   ( ( v & 0x000000FF ) << 24 ) ;
        }
    };

    template <>
    struct byte_swap<std::uint64_t> {
        typedef std::uint64_t value_type;

        static void
        place_swap( value_type &v )
        {
            v = swap( v );
        }

        static
        value_type swap( value_type v )
        {
            return ( ( v & 0xFF00000000000000 ) >> 56) |
                   ( ( v & 0x00FF000000000000 ) >> 40) |
                   ( ( v & 0x0000FF0000000000 ) >> 24) |
                   ( ( v & 0x000000FF00000000 ) >>  8) |
                   ( ( v & 0x00000000FF000000 ) <<  8) |
                   ( ( v & 0x0000000000FF0000 ) << 24) |
                   ( ( v & 0x000000000000FF00 ) << 40) |
                   ( ( v & 0x00000000000000FF ) << 56) ;
        }
    };


    template <typename U>
    std::uint32_t read_big( const U *in )
    {
        const std::uint8_t *i = reinterpret_cast<const std::uint8_t *>(in);
        return ( i[0] << 24 ) |
               ( i[1] << 16 ) |
               ( i[2] <<  8 ) |
               ( i[3]       ) ;
    }

    template <typename U>
    std::uint32_t read_little( const U *in )
    {
        const std::uint8_t *i = reinterpret_cast<const std::uint8_t *>(in);
        return ( i[3] << 24 ) |
               ( i[2] << 16 ) |
               ( i[1] <<  8 ) |
               ( i[0]       ) ;
    }

    template <typename U>
    size_t write_big( std::uint32_t v, U *out )
    {
        std::uint8_t *o = reinterpret_cast<std::uint8_t *>(out);
        o[0] = ( v >> 24 ) & 0xFF;
        o[1] = ( v >> 16 ) & 0xFF;
        o[2] = ( v >>  8 ) & 0xFF;
        o[3] = ( v       ) & 0xFF;
        return sizeof(v);
    }

    template <typename U>
    size_t write_little( std::uint32_t v, U *out )
    {
        std::uint8_t *o = reinterpret_cast<std::uint8_t *>(out);
        o[3] = ( v >> 24 ) & 0xFF;
        o[2] = ( v >> 16 ) & 0xFF;
        o[1] = ( v >>  8 ) & 0xFF;
        o[0] = ( v       ) & 0xFF;
        return sizeof(v);
    }

}}

#endif // BYTE_SWAP_H
