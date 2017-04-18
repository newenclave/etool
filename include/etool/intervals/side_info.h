#ifndef ETOOL_INTERVALS_SIDE_INFO_H
#define ETOOL_INTERVALS_SIDE_INFO_H

#include <cstdint>
#include "etool/intervals/flags.h"

namespace etool { namespace intervals {

    enum class side_name { LEFT = 0, RIGHT = 1, MAX = 2, };

    template <side_name S>
    class side_info;

    template <>
    class side_info<side_name::LEFT> {
    public:
        static const int side = 0;
        static const std::uint16_t def_inf = SIDE_MIN_INF;
    };

    template <>
    class side_info<side_name::RIGHT> {
    public:
        static const int side = 1;
        static const std::uint16_t def_inf = SIDE_MAX_INF;
    };

}}

#endif // SIDE_INFO_H
