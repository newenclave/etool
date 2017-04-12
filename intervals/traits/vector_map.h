#ifndef ETOOL_INTERVALS_TRAITS_VECTOR_MAP_H
#define ETOOL_INTERVALS_TRAITS_VECTOR_MAP_H

#include <algorithm>
#include <map>
#include <vector>

#include "etool/intervals/interval.h"
#include "etool/intervals/traits/sequential_map.h"

namespace etool { namespace intervals { namespace traits {

    template <typename KeyT, typename ValueT>
    struct vector_map_info {
        using key        = KeyT;
        using value      = ValueT;
        using position   = intervals::interval<key>;
        using value_pair = std::pair<position, value>;
        using container  = std::vector<value_pair>;
    };

    template <typename KeyT, typename ValueT>
    using vector_map = sequential_map<vector_map_info<KeyT, ValueT> >;

}}}

#endif // VECTOR_MAP_H
