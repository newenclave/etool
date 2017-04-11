#ifndef ETOOL_INTERVALS_MAP_H
#define ETOOL_INTERVALS_MAP_H

#include <map>

#include "etool/intervals/interval.h"
#include "etool/intervals/operations.h"
#include "etool/intervals/traits/std_map.h"

namespace etool { namespace intervals {

    template <typename KeyT, typename ValueT,
              template <typename, typename> class PosTraitT = traits::std_map>
    class map {
    public:
        using key_type = KeyT;
        using value_type = ValueT;
        using trait_type = PosTraitT<key_type, value_type>;

        using pos = typename trait_type::position;
        using container = typename trait_type::container;

    private:

        template <typename IterT>
        using iter_bool = operations::iter_bool<IterT>;

        template <typename IterT, typename ContT>
        static
        operations::place_pair<IterT> locate( ContT &cont, const pos &p )
        {
            return operations::locate<trait_type, IterT>( cont, p );
        }

        container cont_;
    };

}}

#endif // ETOOL_INTERVALS_
