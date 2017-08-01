#ifndef ETOOL_CHARSET_UTF8_H
#define ETOOL_CHARSET_UTF8_H

#include <cstdint>
#include <string>
#include "etool/details/result.h"

namespace etool { namespace charset {

    struct utf8 {

        using ucs2string = std::basic_string<std::uint16_t>;
        using ucs4string = std::basic_string<std::uint32_t>;

    private:
        static
        std::uint8_t sizes( std::uint8_t val )
        {
            static const std::uint8_t sizes[ ] = {
                1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1,
                1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1,
                1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1,
                1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1,
                1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1,
                1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1,
                1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1,
                1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1,
                0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
                0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
                0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
                0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
                2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2,
                2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2,
                3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3,
                4, 4, 4, 4, 4, 4, 4, 4, 5, 5, 5, 5, 6, 6, 0, 0
            };
            return sizes[val];
        }

        static
        std::uint32_t ranges( std::uint32_t id )
        {
            static const std::uint32_t values[6] = {
                 0x0000007F
                ,0x000007FF
                ,0x0000FFFF
                ,0x001FFFFF
                ,0x03FFFFFF
                ,0x7FFFFFFF
            };
            return values[id];
        }

        static
        std::size_t ucs4_to_utf8( std::uint32_t ucs, std::string &out )
        {
            std::string tmp;
            if( ucs <= ranges(0)  ) {
                tmp.push_back( static_cast<char>(ucs & 0xFF) );
            } else if( ucs <= ranges(1) ) {

                tmp.push_back(static_cast<char>(0xC0 | ( ucs >>  6 )) );
                tmp.push_back(static_cast<char>(0x80 | ( ucs & 0x3F )));

            } else if( ucs <= ranges(2) ) {
                tmp.push_back(static_cast<char>(0xE0 | ( ucs >> 12 )));
                tmp.push_back(static_cast<char>(0x80 | ((ucs >>  6 ) % 0x40)));
                tmp.push_back(static_cast<char>(0x80 | ( ucs % 0x0040)));

            } else if( ucs <= ranges(3) ) {

                tmp.push_back(static_cast<char>(0xF0 | ( ucs >> 18 )));
                tmp.push_back(static_cast<char>(0x80 | ((ucs >> 12 ) & 0x3F)));
                tmp.push_back(static_cast<char>(0x80 | ((ucs >>  6 ) & 0x3F)));
                tmp.push_back(static_cast<char>(0x80 | ( ucs & 0x3F)));

            } else if( ucs <= ranges(4) ) {
                tmp.push_back(static_cast<char>(0xF8 | ( ucs >> 24 )));
                tmp.push_back(static_cast<char>(0x80 | ((ucs >> 18 ) & 0x3F)));
                tmp.push_back(static_cast<char>(0x80 | ((ucs >> 12 ) & 0x3F)));
                tmp.push_back(static_cast<char>(0x80 | ((ucs >>  6 ) & 0x3F)));
                tmp.push_back(static_cast<char>(0x80 | ( ucs & 0x3F)));

            } else if( ucs <= ranges(5) ) {
                tmp.push_back(static_cast<char>(0xFC | ( ucs >> 30 )));
                tmp.push_back(static_cast<char>(0x80 | ((ucs >> 24 ) & 0x3F)));
                tmp.push_back(static_cast<char>(0x80 | ((ucs >> 18 ) & 0x3F)));
                tmp.push_back(static_cast<char>(0x80 | ((ucs >> 12 ) & 0x3F)));
                tmp.push_back(static_cast<char>(0x80 | ((ucs >>  6 ) & 0x3F)));
                tmp.push_back(static_cast<char>(0x80 | ( ucs & 0x3F)));
            }
            out.append( tmp.begin( ), tmp.end( ) );
            return tmp.size( );
        }

        static
        std::size_t utf8_to_ucs4( const char *utf8,
                                  std::size_t available, std::uint32_t *ucs )
        {

            std::uint8_t a = static_cast<std::uint8_t>(utf8[0]);
            std::uint8_t b, c, d, e, f;

            std::uint32_t res = 0;
            std::size_t   used = sizes(a);

            if( available < used || used == 0 ) {
                return 0;
            }

            switch( used ) {
            case 1:
                res = static_cast<std::uint32_t>(utf8[0]);
                break;
            case 2:
                b = static_cast<std::uint8_t>(utf8[1]);
                res = ( static_cast<std::uint32_t>(a-0xC0) <<  6 ) +
                        static_cast<std::uint32_t>(b-0x80);
                break;
            case 3:
                b = static_cast<std::uint8_t>(utf8[1]);
                c = static_cast<std::uint8_t>(utf8[2]);

                res = ( static_cast<std::uint32_t>(a-0xE0) << 12 ) +
                      ( static_cast<std::uint32_t>(b-0x80) <<  6 ) +
                        static_cast<std::uint32_t>(c-0x80);
                break;
            case 4:
                b = static_cast<std::uint8_t>(utf8[1]);
                c = static_cast<std::uint8_t>(utf8[2]);
                d = static_cast<std::uint8_t>(utf8[3]);

                res = ( static_cast<std::uint32_t>(a-0xF0) << 18 ) +
                      ( static_cast<std::uint32_t>(b-0x80) << 12 ) +
                      ( static_cast<std::uint32_t>(c-0x80) <<  6 ) +
                        static_cast<std::uint32_t>(d-0x80);
                break;
            case 5:
                b = static_cast<std::uint8_t>(utf8[1]);
                c = static_cast<std::uint8_t>(utf8[2]);
                d = static_cast<std::uint8_t>(utf8[3]);
                e = static_cast<std::uint8_t>(utf8[4]);

                res =  ( static_cast<std::uint32_t>(a-0xF8) << 24 ) +
                       ( static_cast<std::uint32_t>(b-0x80) << 18 ) +
                       ( static_cast<std::uint32_t>(c-0x80) << 12 ) +
                       ( static_cast<std::uint32_t>(d-0x80) <<  6 ) +
                         static_cast<std::uint32_t>(e-0x80);
                break;
            case 6:
                b = static_cast<std::uint8_t>(utf8[1]);
                c = static_cast<std::uint8_t>(utf8[2]);
                d = static_cast<std::uint8_t>(utf8[3]);
                e = static_cast<std::uint8_t>(utf8[4]);
                f = static_cast<std::uint8_t>(utf8[5]);

                res = ( static_cast<std::uint32_t>(a-0xFC) << 30 ) +
                      ( static_cast<std::uint32_t>(b-0x80) << 24 ) +
                      ( static_cast<std::uint32_t>(c-0x80) << 18 ) +
                      ( static_cast<std::uint32_t>(d-0x80) << 12 ) +
                      ( static_cast<std::uint32_t>(e-0x80) <<  6 ) +
                        static_cast<std::uint32_t>(f-0x80);
                break;
            }
            *ucs = res;
            return used;
        }

    public:
        static
        ucs4string utf8_to_ucs4(const std::string &ut8 )
        {
            ucs4string res;
            for( std::size_t i = 0; i < ut8.size( ); ) {
                std::uint32_t next = 0;
                auto s = utf8_to_ucs4(&ut8[i], ut8.size( ) - i, &next);
                if( s == 0 ) {
                    res.push_back( '?' );
                    ++i;
                } else {
                    res.push_back( next );
                    i += s;
                }
            }
            return res;
        }

        static
        std::string ucs4_to_utf8(const ucs4string &ucs4 )
        {
            std::string res;
            for( auto &n: ucs4 ) {
                ucs4_to_utf8( n, res );
            }
            return res;
        }

        static
        ucs2string utf8_to_ucs2(const std::string &ut8 )
        {
            ucs2string res;
            for( std::size_t i = 0; i < ut8.size( ); ) {
                std::uint32_t next = 0;
                auto s = utf8_to_ucs4( &ut8[i], ut8.size( ) - i, &next);
                if( s == 0 ) {
                    res.push_back( '?' );
                    ++i;
                } else {
                    res.push_back( static_cast<std::uint16_t>(next & 0xFFFF ));
                    i += s;
                }
            }
            return res;
        }
        static
        std::string ucs2_to_utf8(const ucs2string &ucs2 )
        {
            std::string res;
            for( auto &n: ucs2 ) {
                ucs4_to_utf8( static_cast<std::uint32_t>(n), res );
            }
            return res;
        }
    };

}}

#endif // UTF8_H
