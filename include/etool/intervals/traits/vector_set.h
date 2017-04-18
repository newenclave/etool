
#ifndef ETOOL_INTERVALS_TRAITS_VECTOR_SET_H
#define ETOOL_INTERVALS_TRAITS_VECTOR_SET_H

#include <vector>
#include <algorithm>

#include "etool/intervals/interval.h"
#include "etool/intervals/traits/sequential_set.h"

namespace etool { namespace intervals { namespace traits {

    template <typename ValueT>
    struct vector_set_info {
        using key       = ValueT;
        using position  = intervals::interval<key>;
        using container = std::vector<position>;
    };

    template <typename ValueT>
    using vector_set = sequential_set<vector_set_info<ValueT> >;

}}}

#endif // VECTOR_SET_H
