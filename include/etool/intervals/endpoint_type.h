#ifndef ETOOL_INTERVALS_ENDPOINT_TYPE_H
#define ETOOL_INTERVALS_ENDPOINT_TYPE_H

#include <utility>
#include "etool/intervals/attributes.h"

namespace etool { namespace intervals {

    enum class endpoint_name { LEFT = 0, RIGHT = 1 };

    template <typename ValueT, endpoint_name Name>
    class endpoint_type {

        static const endpoint_name opp_side =
                (Name == endpoint_name::LEFT) ? endpoint_name::RIGHT
                                              : endpoint_name::LEFT;
    public:

        using value_type = ValueT;

        static const endpoint_name name = Name;

        struct pointer { };

        static const int id =
                (Name == endpoint_name::LEFT) ? 0 : 1;

        static const attributes def_inf =
                (Name == endpoint_name::LEFT) ? attributes::MIN_INF
                                              : attributes::MAX_INF;
        endpoint_type( value_type &&val )
            :value_(std::move(val))
        { }

        using opposite   = endpoint_type<value_type, opp_side>;
        using open       = attribute<attributes::OPEN>;
        using closed     = attribute<attributes::CLOSE>;

        template <attributes Val>
        struct inf {
            static_assert( (Val == attributes::MIN_INF)
                         | (Val == attributes::MAX_INF),
                           "Side must be MIN_INF or MAX_INF");
            static const attributes value = Val;

            static
            attributes get( )
            {
                return value;
            }
        };

        using default_inf = inf<def_inf>;
        using min_inf     = inf<attributes::MIN_INF>;
        using max_inf     = inf<attributes::MAX_INF>;

        value_type &get( ) noexcept
        {
            return value_;
        }

        value_type &&move( ) noexcept
        {
            return std::move(value_);
        }

    private:

        value_type value_;
    };

}}

#endif // ENDPOINT_TYPE_H
