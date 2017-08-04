#ifndef ETOOL_INTERVALS_TRAITS_INFINITE_H
#define ETOOL_INTERVALS_TRAITS_INFINITE_H

#include <functional>
#include <ostream>
#include "etool/intervals/attributes.h"
#include "etool/intervals/operators.h"

namespace etool { namespace intervals {

    template <typename DomainT, typename Comparator = std::less<DomainT> >
    class value {

        using domain_type = DomainT;

    public:

        template <typename ...Args>
        value( attributes a, Args&& ...args )
            :val_(std::forward<Args>(args)...)
            ,attr_(a)
        { }

        attributes attr( ) const
        {
            return attr_;
        }

        const domain_type &get( ) const
        {
            return val_;
        }

        template <typename ...Args>
        static
        value make( Args&& ...args )
        {
            return value(attributes::OPEN, std::forward<Args>(args)...);
        }

        template <typename ...Args>
        static
        value make( attributes a, Args&& ...args )
        {
            return value( a, std::forward<Args>(args)... );
        }

        template <typename ...Args>
        static
        value make_max_inf( Args&& ...args )
        {
            return value(attributes::MAX_INF, std::forward<Args>(args)...);
        }

        template <typename ...Args>
        static
        value make_min_inf( Args&& ...args )
        {
            return value(attributes::MIN_INF, std::forward<Args>(args)...);
        }

        struct cmp {
            using ocmp = operators::cmp<domain_type, Comparator>;

            static
            bool less( const value &lh, const value &rh )
            {
                if( rh.attr( ) == attributes::MAX_INF ) {
                    return (lh.attr( ) != attributes::MAX_INF);
                } else if( rh.attr( ) == attributes::MIN_INF ) {
                    return false;
                }

                switch(lh.attr( )) {
                case attributes::MAX_INF:
                    return false;
                case attributes::MIN_INF:
                    return rh.attr( ) != attributes::MIN_INF;
                case attributes::OPEN:

                case attributes::CLOSE:
                    break;
                }
                return false;
            }
        };

    private:
        domain_type val_;
        attributes  attr_;
    };

    template <typename D, typename C>
    inline
    std::ostream &operator << ( std::ostream &o, const value<D, C> &v )
    {
        switch (v.attr( )) {
        case attributes::MAX_INF:
            o << "inf";
            break;
        case attributes::MIN_INF:
            o << "-inf";
            break;
        case attributes::OPEN:
        case attributes::CLOSE:
            o << v.get( );
        }
        return o;
    }

}}

#endif // INFINITE_H
