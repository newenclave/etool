#ifndef ETOOL_INTERVALS_OPERATIONS_H
#define ETOOL_INTERVALS_OPERATIONS_H

#include <map>
#include "etool/intervals/interval.h"
#include "etool/intervals/traits/std_map.h"
#include "etool/intervals/traits/std_set.h"

namespace etool { namespace intervals {

    struct operations {

        template <typename IterT>
        struct iter_bool {
            iter_bool( IterT i, bool in )
                :iter(i)
                ,isin(in)
            { }

            iter_bool( const iter_bool & )              = default;
            iter_bool( iter_bool && )                   = default;
            iter_bool &operator = ( iter_bool && )      = default;
            iter_bool &operator = ( const iter_bool & ) = default;

            IterT iter;
            bool  isin = false;
        };

        template <typename ItrT>
        using place_pair = std::pair< iter_bool<ItrT>, iter_bool<ItrT> >;

        template <typename TraitT, typename IterT>
        static
        place_pair<IterT>
        locate( typename TraitT::container &cont,
                const typename TraitT::position &p )
        {
            using trait_type = TraitT;
            using iter       = IterT;
            using iter_acc   = typename trait_type::iterator_access;
            using iter_bool_ = iter_bool<iter>;

            auto b = trait_type::lower_bound( cont, p );

            if( b == trait_type::end( cont ) ) {
                return std::make_pair( iter_bool_( b, false ),
                                       iter_bool_( b, false ) );
            }

            bool bin = false;
            bool ein = false;

            auto e = trait_type::upper_bound( cont, p );

            bin = iter_acc::get(b).contain( p.left( ) );

            if( e != trait_type::begin( cont ) ) {
                auto prev = std::prev( e, 1);
                if( iter_acc::get(prev).contain( p.right( ) ) ) {
                    e = prev;
                    ein = true;
                }
            }

            return std::make_pair( iter_bool_( b, bin ),
                                   iter_bool_( e, ein ) );
        }

    };

}}

#endif // OPERATIONS_H
