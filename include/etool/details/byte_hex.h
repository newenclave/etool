#ifndef ETOOL_DETAILS_BYTE_HEX_H
#define ETOOL_DETAILS_BYTE_HEX_H

#include <cstdint>
#include "etool/details/result.h"

namespace etool { namespace details {

    struct byte_hex {

        using result = detail::result<std::string, std::string>;

        static
        std::uint8_t char2int( char cc )
        {
            std::uint8_t c = static_cast<std::uint8_t>(cc);
            switch (c) {
            case '0': case '1': case '2': case '3': case '4':
            case '5': case '6': case '7': case '8': case '9':
                return c - '0';
            case 'a': case 'b': case 'c':
            case 'd': case 'e': case 'f':
                return c - 'a' + 0xa;
            case 'A': case 'B': case 'C':
            case 'D': case 'E': case 'F':
                return c - 'A' + 0xA;
            }
            return 0xFF;
        }

        static
        result to_hex( const std::string &hex )
        {
            static const char vals[] = {
                '0', '1', '2','3', '4',
                '5', '6', '7','8', '9',
                'A', 'B', 'C','D', 'E', 'F',
            };
            std::string res;
            for( auto &h: hex ) {
                auto val = static_cast<std::uint8_t>(h);
                res.push_back( vals[(val >> 4) & 0xF] );
                res.push_back( vals[(val     ) & 0xF] );
            }
            return result::ok(res);
        }

        static
        result from_hex( const std::string &hex )
        {
            return from_hex(hex.c_str( ), hex.size( ));
        }

        static
        result from_hex( const char *hex, std::size_t len )
        {
            std::string res;
            if( len % 2 != 0 ) {
                return result::fail( "Bad serialized string." );
            }
            for( std::size_t i=0; i<len; i += 2 ) {
                auto h = char2int( hex[i] );
                auto l = char2int( hex[i + 1] );
                if( (h == 0xFF) || (l == 0xFF) ) {
                    return result::fail( "Bad serialized string." );
                }
                res.push_back( static_cast<char>( h ) );
                res.push_back( static_cast<char>( l ) );
            }
            return result::ok(res);
        }

    };

}}

#endif // BYTE_HEX_H
