#ifndef ETOOL_DUMPER_DUMP_HPP
#define ETOOL_DUMPER_DUMP_HPP

#include <stdint.h>
#include <ostream>

namespace etool { namespace dumper {

    typedef uint8_t  ui8;
    typedef uint16_t ui16;
    typedef uint32_t ui32;
    typedef uint64_t ui64;

    struct info {

        static const int line_size   = 80;
        static const int data_shift  = 63;
        static const int elements    = 16;

        static
        void getx( ui8 c, char *res )
        {
            static const char hex[ ] = { '0', '1', '2', '3',
                                         '4', '5', '6', '7',
                                         '8', '9', 'a', 'b',
                                         'c', 'd', 'e', 'f' };
            res[0] = hex[(c >> 4) & 0xF];
            res[1] = hex[ c       & 0xF];
        }

        static
        void getX( ui8 c, char *res )
        {
            static const char hex[ ] = { '0', '1', '2', '3',
                                         '4', '5', '6', '7',
                                         '8', '9', 'A', 'B',
                                         'C', 'D', 'E', 'F' };
            res[0] = hex[(c >> 4) & 0xF];
            res[1] = hex[ c       & 0xF];
        }

        static
        void get_( char *res )
        {
            res[0] = res[1] = '-';
        }

        static
        bool printable( ui8 c )
        {
            return (c >= 32) && (c <= 126);
        }

        static
        void copy_data( const char *input, size_t len, char *output )
        {
            for( size_t i = 0; i < 16; i++, input++, output++ ) {
                if( i < len ) {
                    if( printable( *input ) ) {
                        *output = *input;
                    } else {
                        *output = '.';
                    }
                } else {
                    *output = ' ';
                }
            }
        }

        static
        size_t buffer_size( size_t len )
        {
            size_t count = (len / elements) + (len % elements > 0);
            return count * (line_size + 1);
        }

    };

    struct ui16_to_hex {

        static const ui16 ma  = 0xFF;
        static const ui16 sh  = 8;
        static const ui16 si  = 2;

        static
        void getx( ui16 c, char *res )
        {
            info::getx( static_cast<ui8>((c >> sh) & ma), res );
            info::getx( static_cast<ui8>( c        & ma), res + si );
        }

        static
        void getX( ui16 c, char *res )
        {
            info::getX( static_cast<ui8>((c >> sh) & ma), res );
            info::getX( static_cast<ui8>( c        & ma), res + si );
        }

        static
        void get_( char *res )
        {
            info::get_(res);
            info::get_(res + si);
        }
    };

    struct ui32_to_hex {

        static const ui16 ma  = 0xFFFF;
        static const ui16 sh  = 16;
        static const ui16 si  = 4;

        static
        void getx( ui32 c, char *res )
        {
            ui16_to_hex::getx( static_cast<ui16>((c >> sh) & ma), res );
            ui16_to_hex::getx( static_cast<ui16>( c        & ma), res + si );
        }

        static
        void getX( ui32 c, char *res )
        {
            ui16_to_hex::getX( static_cast<ui16>((c >> sh) & ma), res );
            ui16_to_hex::getX( static_cast<ui16>( c        & ma), res + si );
        }

        static
        void get_( char *res )
        {
            ui16_to_hex::get_(res);
            ui16_to_hex::get_(res + si);
        }

    };

    struct ui64_to_hex {

        static const ui32 ma  = 0xFFFFFFFF;
        static const ui32 sh  = 32;
        static const ui32 si  = 8;

        static
        void getx( ui64 c, char *res )
        {
            ui32_to_hex::getx( static_cast<ui32>((c >> sh) & ma), res );
            ui32_to_hex::getx( static_cast<ui32>( c        & ma), res + si );
        }

        static
        void getX( ui64 c, char *res )
        {
            ui32_to_hex::getX( static_cast<ui32>((c >> sh) & ma), res );
            ui32_to_hex::getX( static_cast<ui32>( c        & ma), res + si );
        }

        static
        void get_( char *res )
        {
            ui32_to_hex::get_(res);
            ui32_to_hex::get_(res + si);
        }
    };

    template <typename T>
    struct T_to_hex;

    template <>
    struct T_to_hex<ui8> {
        typedef info hex_type;
    };

    template <>
    struct T_to_hex<char> {
        typedef info hex_type;
    };

    template <>
    struct T_to_hex<ui16> {
        typedef ui16_to_hex hex_type;
    };

    template <>
    struct T_to_hex<ui32> {
        typedef ui32_to_hex hex_type;
    };

    template <>
    struct T_to_hex<ui64> {
        typedef ui64_to_hex hex_type;
    };

    template <typename T = ui8>
    struct make {

        typedef T value_type;
        typedef typename T_to_hex<value_type>::hex_type value_info;

        static
        void line( uint32_t id, const char *input, size_t len, char *output )
        {
            static const size_t value_size = sizeof(value_type);
            const value_type *p = reinterpret_cast<const value_type *>(input);

            std::fill( output, output + info::line_size, ' ' );

            info::copy_data( input, len, output + info::data_shift );

            ui32_to_hex::getX( id, output );
            output += 8;

            for( size_t i = 0; i < info::elements; i += value_size, p++ ) {
                if( i % 8 == 0 ) {
                    output += 2;
                } else {
                    output += 1;
                }
                if( (i + value_size - 1) < len ) {
                    value_info::getX( *p, output );
                } else {
                    value_info::get_( output );
                }

                output += (value_size * 2);
            }
        }

        static
        void all( const void *input, size_t len, char *output )
        {
            const char *inp = static_cast<const char *>(input);
            for( size_t id = 0; id<len; id += info::elements ) {
                line( id, &inp[id], len - id, output );
                output[info::line_size] = '\n';
                output += (info::line_size + 1);
            }
        }

        template <typename StreamType>
        static
        StreamType &all( const void *input, size_t len, StreamType &os )
        {
            const char *inp = static_cast<const char *>(input);
            for( size_t id = 0; id < len; id += info::elements ) {
                if( id ) {
                    os << "\n";
                }
                char buf[info::line_size + 1];
                line( id, &inp[id], len - id, buf );
                buf[info::line_size] = '\0';
                os << &buf[0];
            }
            return os;
        }

        static
        size_t buffer_size( size_t len )
        {
            return info::buffer_size( len );
        }

        static
        std::string to_hex( const void *input,  size_t len,
                            std::string postfix = "",
                            std::string prefix  = "" )
        {
            static const size_t element_buf_len = sizeof(T) * 2 + 1;
            union {
                char buf[element_buf_len];
                value_type val;
            } bv;

            bv.buf[element_buf_len - 1] = '\0';
            const value_type *p = reinterpret_cast<const value_type *>(input);

            std::string res;
            for( size_t i=0; i<len; i += sizeof(T), p++ ) {
                if( i > 0 ) {
                    res += postfix;
                }
                res += prefix;
                value_info::getX( *p, bv.buf );
                res.append( &bv.buf[0], &bv.buf[element_buf_len - 1] );
            }
            return std::move(res);
        }
    };

}}

#endif // DUMP_HPP
