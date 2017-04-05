#ifndef ETOOL_SIZEPACK_BLOCKCHAIN_VARINT_H
#define ETOOL_SIZEPACK_BLOCKCHAIN_VARINT_H

#include <algorithm>

#include "etool/details/byte_order.h"

namespace etool { namespace sizepack {

    struct blockchain_varint {

        typedef std::uint64_t size_type;

        static const size_t max_length = sizeof(size_type) + 1;
        static const size_t min_length = 1;

        using u16_little = details::byte_order_little<std::uint16_t>;
        using u32_little = details::byte_order_little<std::uint32_t>;
        using u64_little = details::byte_order_little<std::uint64_t>;

//        enum {
//            BBP_VARINT16 = 0xFD,
//            BBP_VARINT32 = 0xFE,
//            BBP_VARINT64 = 0xFF
//        } bbp_varint_t;

        static
        size_t len_by_prefix( std::uint8_t prefix )
        {
            switch (prefix) {
            case 0xFF:
                return sizeof(std::uint64_t) + 1;
            case 0xFE:
                return sizeof(std::uint32_t) + 1;
            case 0xFD:
                return sizeof(std::uint16_t) + 1;
            default:
                break;
            }
            return sizeof(std::uint8_t);
        }

        static
        bool valid_length( size_t len )
        {
            return (len >= min_length) && (len <= max_length);
        }

        template <typename IterT>
        static
        size_t packed_length( IterT begin, const IterT &end )
        {
            size_t dist   = std::distance(begin, end);
            size_t len    = len_by_prefix( *begin );
            return (dist >= len) ? len : 0;
        }

        template <typename IterT>
        static
        bool valid_packed( const IterT &begin, const IterT &end )
        {
            return valid_length( packed_length( begin, end ) );
        }

        static
        size_t result_length( size_type len )
        {
            if( len < 0xFD ) {
                return sizeof(std::uint8_t);
            } else if( len <= 0xFFFF ) {
                return sizeof(std::uint16_t) + 1;
            } else if( len <= 0xFFFFFFFF ) {
                return sizeof(std::uint32_t) + 1;
            } else {
                return sizeof(std::uint64_t) + 1;
            }
        }

        static
        size_t pack( size_type size, void *result )
        {
            std::uint8_t *res  = reinterpret_cast<std::uint8_t *>(result);
            if( size < 0xFD ) {
                res[0] = static_cast<std::uint8_t>(size);
                return 1;
            } else if( size <= 0xFFFF ) {
                res[0] = static_cast<std::uint8_t>(0xFD);
                return u16_little::write( size, &res[1] ) + 1;
            } else if( size <= 0xFFFFFFFF ) {
                res[0] = static_cast<std::uint8_t>(0xFE);
                return u32_little::write( size, &res[1] ) + 1;
            } else {
                res[0] = static_cast<std::uint8_t>(0xFF);
                return u64_little::write( size, &res[1] ) + 1;
            }
        }

        static
        void pack( size_type size, std::string &res )
        {
            std::string tmp(result_length(size), '\0');
            pack( size, &tmp[0] );
            res.swap( tmp );
        }

        static
        std::string pack( size_type size )
        {
            std::string res;
            pack( size, res );
            return std::move(res);
        }

        static
        void append( size_type size, std::string &res )
        {
            size_t last = res.size( );
            res.resize(last + result_length(size));
            pack( size, &res[last] );
        }

        template <typename IterT>
        static size_type unpack( IterT begin, const IterT &end )
        {
            size_type res = 0;
            size_t shift = 0;
            size_t len = len_by_prefix( *begin++ );
            for( ; begin != end; ++begin, --len, shift += 8 ) {
                res |= static_cast<size_type>(*begin) << shift;
            }
            return res;
        }
    };

}}

#endif // BLOCKCHAIN_VARINT_H
