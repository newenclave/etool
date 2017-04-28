#ifndef ETOOL_INTERVALS_MAP_H
#define ETOOL_INTERVALS_MAP_H

#include "etool/intervals/tree.h"
#include "etool/intervals/traits/std_map.h"

namespace etool { namespace intervals {
    template <typename KeyT, typename ValueT, typename Comp = std::less<KeyT> >
    using map = tree<traits::std_map<KeyT, ValueT, Comp> >;
}}

#endif // MAP_H
