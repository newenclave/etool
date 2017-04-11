#ifndef ETOOL_INTERVALS_FLAGS_H
#define ETOOL_INTERVALS_FLAGS_H


namespace etool { namespace intervals {

    enum include_flags {
        INCLUDE_NONE  = 0x00,
        INCLUDE_RIGTH = 0x01,
        INCLUDE_LEFT  = 0x02,
        INCLUDE_BOTH  = INCLUDE_RIGTH | INCLUDE_LEFT,
    };

}}

#endif // FLAGS_H
