#ifndef ETOOL_CONDOLE_GEOMETRY_H
#define ETOOL_CONDOLE_GEOMETRY_H

#include <sstream>
#include <string>
#include <iostream>

#ifdef _WIN32
#define NOMINMAX
#include <windows.h>
#undef NOMINMAX
#else
#include <sys/ioctl.h>
#include <unistd.h>
#endif

namespace etool { namespace console {
    struct info {

        struct coord {

            unsigned x = 0;
            unsigned y = 0;

            coord( unsigned xin, unsigned yin )
                :x(xin)
                ,y(yin)
            { }
            coord( ) = default;
        };

#ifdef _WIN32

        static
        void clear( )
        {
            HANDLE sout = GetStdHandle(STD_OUTPUT_HANDLE);
            COORD coord = {0, 0};
            DWORD count;
            CONSOLE_SCREEN_BUFFER_INFO csbi;
            GetConsoleScreenBufferInfo( sout, &csbi );
            FillConsoleOutputCharacter( sout, ' ',
                                        csbi.dwSize.X * csbi.dwSize.Y,
                                        coord, &count);
            SetConsoleCursorPosition( sout, coord );
        }

        static
        coord size( )
        {
            CONSOLE_SCREEN_BUFFER_INFO csbi;

            int columns;
            int rows;

            GetConsoleScreenBufferInfo(GetStdHandle(STD_OUTPUT_HANDLE), &csbi);

            columns = csbi.srWindow.Right  - csbi.srWindow.Left + 1;
            rows    = csbi.srWindow.Bottom - csbi.srWindow.Top + 1;

            return pos::coord( columns, rows );
        }

        static
        void set_pos( int x, int y )
        {
            COORD coord;

            coord.X = x;
            coord.Y = y;

            SetConsoleCursorPosition( GetStdHandle(STD_OUTPUT_HANDLE), coord );
        }
#else

        static
        coord size( )
        {
            struct winsize w;
            ioctl( STDOUT_FILENO, TIOCGWINSZ, &w );
            return coord( w.ws_col, w.ws_row );
        }

        static
        void clear( )
        {
            auto xy = size( );
            std::string ws( xy.x, ' ' );

            for( int i = 0; i < static_cast<int>(xy.y); i++ ) {
                set_pos( 0, i );
                std::cout.write( ws.c_str( ),
                                 static_cast<std::streamsize>(ws.size( )) );
            }
            // clear command that does "write("\E[H\E[2J", 7 )" we don't want
            std::cout.flush( );
        }

        static
        void set_pos( int x, int y )
        {
            std::ostringstream oss;
            oss << "\033[" << (y + 1) << ";" << (x + 1) << "H";
            std::string s(oss.str( ));
            std::cout.write( s.c_str( ),
                             static_cast<std::streamsize>(s.size( )) );

            //write( STDIN_FILENO, s.c_str( ), s.size( ) );
        }

        //    void set_size( const coord &cs )
        //    {
        //        struct winsize w = { 0, 0, 0, 0 };
        //        w.ws_col = cs.first;
        //        w.ws_row = cs.second;
        //        ioctl( 0, TIOCSWINSZ, &w);
        //    }
#endif

    };
}}

#endif // GEOMETRY_H
