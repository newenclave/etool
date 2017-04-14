#ifndef ETOOL_INTERVALS_FLAGS_H
#define ETOOL_INTERVALS_FLAGS_H


namespace etool { namespace intervals {

    enum include_flags {

        SIDE_SHIFT        = 0x10,   /// 16 bit

        SIDE_CLOSE        = 0x00,
        SIDE_OPEN         = 0x01,

        SIDE_MIN_INF      = 0x02,
        SIDE_MAX_INF      = 0x04,

        SIDE_OPEN_MIN_INF = SIDE_MIN_INF | SIDE_OPEN,
        SIDE_OPEN_MAX_INF = SIDE_MAX_INF | SIDE_OPEN,

        SIDE_RIGHT_CLOSE  = SIDE_CLOSE,
        SIDE_RIGHT_OPEN   = SIDE_OPEN,
        SIDE_RIGHT_IMIN   = SIDE_MIN_INF,
        SIDE_RIGHT_IMAX   = SIDE_MAX_INF,
        SIDE_RIGHT_INF    = SIDE_RIGHT_IMAX,

        SIDE_LEFT_CLOSE   = SIDE_CLOSE    << SIDE_SHIFT,
        SIDE_LEFT_OPEN    = SIDE_OPEN     << SIDE_SHIFT,
        SIDE_LEFT_IMIN    = SIDE_MIN_INF  << SIDE_SHIFT,
        SIDE_LEFT_IMAX    = SIDE_MAX_INF  << SIDE_SHIFT,
        SIDE_LEFT_INF     = SIDE_LEFT_IMIN,

        SIDE_BOTH_INF     = SIDE_LEFT_IMIN  | SIDE_RIGHT_IMAX,
        SIDE_BOTH_CLOSE   = SIDE_LEFT_CLOSE | SIDE_RIGHT_CLOSE,
        SIDE_BOTH_OPEN    = SIDE_LEFT_OPEN  | SIDE_RIGHT_OPEN,

        SIDE_MASK         = 0xFFFF, /// 16 bit

    };

}}

#endif // FLAGS_H
