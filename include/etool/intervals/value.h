#ifndef ETOOL_INTERVALS_TRAITS_INFINITE_H
#define ETOOL_INTERVALS_TRAITS_INFINITE_H

#include <functional>
#include "etool/intervals/attributes.h"
#include "etool/intervals/operators.h"

namespace etool { namespace intervals {

    template <typename DomainT, typename Comparator = std::less<DomainT> >
    class value {

        using domain_type = DomainT;
        using cmp = operators::cmp<domain_type, Comparator>;

    public:

        attributes attr( ) const
        {
            return attr_;
        }

        const domain_type &get( ) const
        {
            return val_;
        }

    private:
        domain_type val_;
        attributes  attr_;
    };

}}

#endif // INFINITE_H
