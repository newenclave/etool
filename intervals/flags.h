#ifndef ETOOL_INTERVALS_FLAGS_H
#define ETOOL_INTERVALS_FLAGS_H


namespace etool { namespace intervals {

    enum include_flags {
        INCLUDE_NONE    = 0x00,
        INCLUDE_RIGTH   = 0x01,
        INCLUDE_LEFT    = 0x02,
        INF_PLUS_RIGHT  = 0x04,
        INF_MINUS_LEFT  = 0x08,

        INF_MINUS_RIGHT = 0x10,
        INF_PLUS_LEFT   = 0x20,

        INCLUDE_BOTH    = INCLUDE_RIGTH | INCLUDE_LEFT,
        INF_BOTH        = INF_PLUS_RIGHT     | INF_MINUS_LEFT,
    };

}}

#endif // FLAGS_H
