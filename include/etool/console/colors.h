#ifndef ETOOL_CONDOLE_COLORS_H
#define ETOOL_CONDOLE_COLORS_H

#include <iostream>
#ifdef _WIN32
#include <windows.h>
#endif

namespace etool { namespace console {

namespace ccout {

    enum color_value {
         COLOR_NONE     = 0
        ,COLOR_RED      = 1
        ,COLOR_GREEN    = 2
        ,COLOR_BLUE     = 3
        ,COLOR_YELLOW   = 4
        ,COLOR_WHITE    = 5
        ,COLOR_CYAN     = 6
    };

#ifdef _WIN32

    enum colors_enum
    {
        Black       = 0,
        Grey        = FOREGROUND_INTENSITY,
        LightGrey   = FOREGROUND_RED   | FOREGROUND_GREEN
                                       | FOREGROUND_BLUE,
        White       = FOREGROUND_RED   | FOREGROUND_GREEN
                                       | FOREGROUND_BLUE
                                       | FOREGROUND_INTENSITY,
        Blue        = FOREGROUND_BLUE,
        Green       = FOREGROUND_GREEN,
        Cyan        = FOREGROUND_GREEN | FOREGROUND_BLUE,
        Red         = FOREGROUND_RED,
        Purple      = FOREGROUND_RED   | FOREGROUND_BLUE,
        LightBlue   = FOREGROUND_BLUE  | FOREGROUND_INTENSITY,
        LightGreen  = FOREGROUND_GREEN | FOREGROUND_INTENSITY,
        LightCyan   = FOREGROUND_GREEN | FOREGROUND_BLUE
                                       | FOREGROUND_INTENSITY,
        LightRed    = FOREGROUND_RED   | FOREGROUND_INTENSITY,
        LightPurple = FOREGROUND_RED   | FOREGROUND_BLUE
                                       | FOREGROUND_INTENSITY,
        Orange      = FOREGROUND_RED   | FOREGROUND_GREEN,
        Yellow      = FOREGROUND_RED   | FOREGROUND_GREEN
                                       | FOREGROUND_INTENSITY,
    };

    struct color_map {
        static
        unsigned get( std::size_t id )
        {
            static const unsigned val[ ] = {
                LightGrey,
                LightRed, LightGreen, LightBlue,
                Yellow, White, Cyan
            };
            return val[id];
        }
        static
        std::size_t count( )
        {
            return 7;
        }
    }
#else

    struct color_map {
        static
        const char *get( std::size_t id )
        {
            static const char * cp_none      = "\033[0m";
            //static const char * cp_none      = "\x1b[0m";
            //static const char * cp_black     = "\x1b[30;1m";
            static const char * cp_red       = "\x1b[31;1m";
            static const char * cp_green     = "\x1b[32;1m";
            static const char * cp_yellow    = "\x1b[33;1m";
            //static const char * cp_orange    = "\x1b[33;1m";
            static const char * cp_blue      = "\x1b[34;1m";
            //static const char * cp_purple    = "\x1b[35;1m";
            static const char * cp_cyan      = "\x1b[36;1m";
            static const char * cp_white     = "\x1b[37;1m";

            static const char *val[ ] = {
                cp_none,
                cp_red, cp_green, cp_blue,
                cp_yellow, cp_white, cp_cyan
            };
            return val[id];
        }
        static
        std::size_t count( )
        {
            return 7;
        }
    };
#endif

    inline
    std::ostream &set_stream_color( std::ostream &s, unsigned color )
    {
        if( color >= color_map::count( ) ) {
            color = COLOR_NONE;
        }
#ifdef _WIN32
        SetConsoleTextAttribute( GetStdHandle( STD_OUTPUT_HANDLE ),
                                 color_map::get(color) );
#else
        s << color_map::get(color);
#endif
        return s;
    }

    inline
    std::ostream &light ( std::ostream &s )
    {
        return set_stream_color(s, COLOR_WHITE);
    }

    inline
    std::ostream &red   ( std::ostream &s )
    {
        return set_stream_color(s, COLOR_RED);
    }

    inline
    std::ostream &green ( std::ostream &s )
    {
        return set_stream_color(s, COLOR_GREEN);
    }

    inline
    std::ostream &blue  ( std::ostream &s )
    {
        return set_stream_color(s, COLOR_BLUE);
    }

    inline
    std::ostream &yellow( std::ostream &s )
    {
        return set_stream_color(s, COLOR_YELLOW);
    }

    inline
    std::ostream &none  ( std::ostream &s )
    {
        return set_stream_color(s, COLOR_NONE);
    }

    inline
    std::ostream &cyan  ( std::ostream &s )
    {
        return set_stream_color(s, COLOR_CYAN);
    }
}

}}

#endif // COLORS_H
