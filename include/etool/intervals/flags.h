#ifndef ETOOL_INTERVALS_FLAGS_H
#define ETOOL_INTERVALS_FLAGS_H


namespace etool { namespace intervals {

    enum include_flags {
        SIDE_CLOSE        = 0x00,
        SIDE_OPEN         = 0x01,
        SIDE_MIN_INF      = 0x02,
        SIDE_MAX_INF      = 0x03,
        SIDE_MAXIMUM
    };

}}

#endif // FLAGS_H
