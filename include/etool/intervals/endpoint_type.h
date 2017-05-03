#ifndef ETOOL_INTERVALS_ENDPOINT_TYPE_H
#define ETOOL_INTERVALS_ENDPOINT_TYPE_H

#include <utility>
#include "etool/intervals/attributes.h"

namespace etool { namespace intervals {

    enum class endpoint_name { LEFT = 0, RIGHT = 1 };

    template <endpoint_name Name>
    struct endpoint_type {

        static const endpoint_name name = Name;

        struct pointer { };

        static const int id =
                (Name == endpoint_name::LEFT) ? 0 : 1;

        static const attributes def_inf =
                (Name == endpoint_name::LEFT) ? attributes::MIN_INF
                                              : attributes::MAX_INF;

        static const endpoint_name oppisite_name =
                (Name == endpoint_name::LEFT) ? endpoint_name::RIGHT
                                              : endpoint_name::LEFT;

        using opposite = endpoint_type<oppisite_name>;
    };

}}

#endif // ENDPOINT_TYPE_H
