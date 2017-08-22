#ifndef ETOOL_DETAILS_LEXICAL_CAST_H
#define ETOOL_DETAILS_LEXICAL_CAST_H

#include <sstream>

namespace etool { namespace details {

    template <typename ToT, typename FromT>
    struct lexical_cast_impl;

    template <typename FloatT, typename CharT>
    struct lexical_cast_floating {

        using float_type = FloatT;
        using char_type  = CharT;

        static
        bool valid_for_dec( int cc )
        {
            return ( '0' <= cc ) && ( cc <= '9' );
        }

        static
        bool is_gap( int cc )
        {
            return cc == '_';
        }

        static
        bool is_exp( int cc )
        {
            return ( cc == 'e' || cc == 'E' );
        }

        static
        float_type cast( const char_type *s )
        {
            float_type  a = float_type(0.0);
            int         e = 0;
            int         c = 0;
            float_type  m = float_type(1);

            if( *s == '-' ) {
                m = float_type( -1 );
                ++s;
            }

            while( *s && valid_for_dec(c = *s++) ) {
                if( !is_gap(c) ) {
                    a = a * float_type(10.0) + (c - '0');
                }
            }

            if( c == '.' ) {
                while( *s && valid_for_dec(c = *s++)) {
                    if( !is_gap(c) ) {
                        a = a * float_type(10.0) + (c - '0');
                        e = e - 1;
                    }
                }
            }

            if( is_exp( c ) ) {

                int sign = 1;
                int i = 0;

                c = *s++;

                if( c == '+' ) {
                    c = *s++;
                } else if( c == '-' ) {
                    c = *s++;
                    sign = -1;
                }

                while( valid_for_dec( c ) ) {
                    if( is_gap(c) ) {
                        c = *s++;
                    }
                    i = i * 10 + (c - '0');
                    if( !*s ) {
                        break;
                    } else {
                        c = *s++;
                    }
                }
                e += i*sign;
            }

            while( e > 0 ) {
                a *= float_type(10.0);
                e--;
            }

            while( e < 0 ) {
                a *= float_type(0.1);
                e++;
            }

            return a * m;
        }
    };

    template <typename T>
    struct lexical_cast_impl<T, T> {
        static
        T cast( T from )
        {
            return from;
        }
    };

    template <>
    struct lexical_cast_impl<std::string, const char *> {
        static
        std::string cast( const char *from )
        {
            return from;
        }
    };

    template <>
    struct lexical_cast_impl<std::wstring, const std::wstring::value_type *> {
        static
        std::wstring cast( const std::wstring::value_type *from )
        {
            return from;
        }
    };

    template <typename T>
    struct lexical_cast_impl<std::string, T> {
        static
        std::string cast( T from )
        {
            std::ostringstream oss;
            oss << from;
            return oss.str( );
        }
    };

    template <typename T>
    struct lexical_cast_impl<std::wstring, T> {
        static
        std::wstring cast( T from )
        {
            std::wostringstream oss;
            oss << from;
            return oss.str( );
        }
    };

    template <typename T>
    struct lexical_cast_impl<T, std::wstring> {
        static
        T cast( std::wstring from )
        {
            std::wstringstream wss;
            wss << from;
            T res { };
            wss >> res;
            return res;
        }
    };

    template <typename T>
    struct lexical_cast_impl<T, const std::wstring::value_type *> {
        static
        T cast( const std::wstring::value_type *from )
        {
            std::wstringstream wss;
            wss << from;
            T res { };
            wss >> res;
            return res;
        }
    };

    /////////////// Floating
    /// Wstring
    template <>
    struct lexical_cast_impl<float, std::wstring> {
        static
        float cast( const std::wstring &from )
        {
            using val_type = std::wstring::value_type;
            return lexical_cast_floating<float, val_type>::cast(from.c_str( ));
        }
    };

    template <>
    struct lexical_cast_impl<float, const std::wstring::value_type  *> {
        using char_type = std::wstring::value_type;
        static
        float cast( const char_type *from )
        {
            return lexical_cast_floating<float, char_type>::cast(from);
        }
    };

    template <>
    struct lexical_cast_impl<double, std::wstring> {
        static
        double cast( const std::wstring &from )
        {
            using val_type = std::wstring::value_type;
            return lexical_cast_floating<double, val_type>::cast(from.c_str( ));
        }
    };

    template <>
    struct lexical_cast_impl<double, const std::wstring::value_type  *> {
        using char_type = std::wstring::value_type;
        static
        double cast( const char_type *from )
        {
            return lexical_cast_floating<double, char_type>::cast(from);
        }
    };

    template <>
    struct lexical_cast_impl<long double, std::wstring> {
        static
        long double cast( const std::wstring &from )
        {
            using val_type = std::wstring::value_type;
            return lexical_cast_floating<long double, val_type>
                                        ::cast(from.c_str( ));
        }
    };

    template <>
    struct lexical_cast_impl<long double, const std::wstring::value_type  *> {
        using char_type = std::wstring::value_type;
        static
        long double cast( const char_type *from )
        {
            return lexical_cast_floating<long double, char_type>::cast(from);
        }
    };

    /// string
    template <>
    struct lexical_cast_impl<float, std::string> {
        static
        float cast( const std::string &from )
        {
            using val_type = std::string::value_type;
            return lexical_cast_floating<float, val_type>::cast(from.c_str( ));
        }
    };

    template <>
    struct lexical_cast_impl<float, const char *> {
        static
        float cast( const char *from )
        {
            return lexical_cast_floating<float, char>::cast(from);
        }
    };

    template <>
    struct lexical_cast_impl<double, std::string> {
        static
        double cast( const std::string &from )
        {
            using val_type = std::string::value_type;
            return lexical_cast_floating<double, val_type>::cast(from.c_str( ));
        }
    };

    template <>
    struct lexical_cast_impl<double, const char *> {
        static
        double cast( const char *from )
        {
            return lexical_cast_floating<double, char>::cast(from);
        }
    };

    template <>
    struct lexical_cast_impl<long double, std::string> {
        static
        long double cast( const std::string &from )
        {
            using val_type = std::string::value_type;
            return lexical_cast_floating<long double, val_type>
                    ::cast(from.c_str( ));
        }
    };

    template <>
    struct lexical_cast_impl<long double, const char *> {
        static
        long double cast( const char *from )
        {
            return lexical_cast_floating<long double, char>::cast(from);
        }
    };
    /////////////// Floating


    /////////////// Common
    template <typename ToT, typename FromT>
    struct lexical_cast_impl {
        static
        ToT cast( FromT from )
        {
            std::stringstream sstr;
            sstr << from;
            ToT res { };
            sstr >> res;
            return res;
        }
    };

    template <typename ToT, typename FromT>
    inline
    ToT lexical_cast( FromT from )
    {
        return lexical_cast_impl<ToT, FromT>::cast( std::move(from) );
    }

}}

#endif // LEXICAL_CAST_H
