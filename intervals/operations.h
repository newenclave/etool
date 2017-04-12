#ifndef ETOOL_INTERVALS_OPERATIONS_H
#define ETOOL_INTERVALS_OPERATIONS_H

#include <map>
#include "etool/intervals/interval.h"
#include "etool/intervals/traits/std_map.h"
#include "etool/intervals/traits/std_set.h"

namespace etool { namespace intervals {

    template <typename TraitT>
    struct operations {

        using trait_type = TraitT;

        using position = typename trait_type::position;

        template <typename IterT>
        struct iter_bool {
            iter_bool( IterT i, bool in, bool bo )
                :iter(i)
                ,inside(in)
                ,border(bo)
            { }

            iter_bool( const iter_bool & )              = default;
            iter_bool( iter_bool && )                   = default;
            iter_bool &operator = ( iter_bool && )      = default;
            iter_bool &operator = ( const iter_bool & ) = default;

            IterT iter;
            bool  inside = false;
            bool  border = false;
        };

        template <typename ItrT>
        using place_pair = std::pair< iter_bool<ItrT>, iter_bool<ItrT> >;

        template <typename ItrT>
        static
        bool has_left_border( ItrT &itr, const typename TraitT::position &p )
        {
            using iter_acc = typename trait_type::iterator_access;
            ItrT prev = std::prev(itr, 1);
            return p.left_neighbor(*iter_acc::get(prev));
        }

        template <typename ItrT>
        static
        bool has_right_border( ItrT &itr, const typename TraitT::position &p )
        {
            using iter_acc = typename trait_type::iterator_access;
            return p.right_neighbor(*iter_acc::get(itr));
        }

        template <typename IterT, typename ContT>
        static
        place_pair<IterT>
        locate( ContT &cont, const position &p )
        {
            using iter       = IterT;
            using iter_acc   = typename trait_type::iterator_access;
            using iter_bool_ = iter_bool<iter>;

            auto b = trait_type::lower_bound( cont, p );

            if( b == trait_type::end( cont ) ) {
                bool border = false;
                if( b != trait_type::begin( cont ) ) {
                    border = has_left_border(b, p);
                }
                return std::make_pair( iter_bool_( b, false, border ),
                                       iter_bool_( b, false, false ) );
            }

            bool bin = false;
            bool ein = false;

            bool bor = false;
            bool eor = false;

            auto e = trait_type::upper_bound( cont, p );

            if( b != trait_type::begin(cont) ) {
                bor = has_left_border( b, p );
            }

            bin = iter_acc::get(b)->contain( p.left( ) );

            if( e != trait_type::begin( cont ) ) {
                auto prev = std::prev( e, 1);
                if( iter_acc::get(prev)->contain( p.right( ) ) ) {
                    e = prev;
                    ein = true;
                }
            }

            if( e != trait_type::end(cont) ) {
                eor = has_right_border( e, p );
            }

            return std::make_pair( iter_bool_( b, bin, bor ),
                                   iter_bool_( e, ein, eor ) );
        }

    };

}}

#endif // OPERATIONS_H
