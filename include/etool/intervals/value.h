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

        value( )                             = default;
        value( const value & )               = default;
        value( value && )                    = default;
        value & operator = ( const value & ) = default;
        value & operator = ( value && )      = default;

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
        value make( attributes a, Args&& ...args )
        {
            return value( a, std::forward<Args>(args)... );
        }

        template <typename ...Args>
        static
        value make_open( Args&& ...args )
        {
            return value( attributes::OPEN, std::forward<Args>(args)...);
        }

        template <typename ...Args>
        static
        value make_closed( Args&& ...args )
        {
            return value( attributes::CLOSE, std::forward<Args>(args)...);
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

        bool is_open( ) const
        {
            return attr( ) == attributes::OPEN;
        }

        bool is_closed( ) const
        {
            return attr( ) == attributes::CLOSE;
        }

        bool is_min( ) const
        {
            return attr( ) == attributes::MIN_INF;
        }

        bool is_max( ) const
        {
            return attr( ) == attributes::MAX_INF;
        }

        struct cmp {

            using ocmp = operators::cmp<domain_type, Comparator>;

            static
            bool less( const value &lh, const value &rh )
            {
                if( rh.is_max( ) ) {
                    return !lh.is_max( );
                } else if( rh.is_min( ) ) {
                    return false;
                }

                if( lh.is_max( ) ) {
                    return false;
                } else if( lh.is_min( ) ) {
                    return !rh.is_min( );
                }

                if( lh.is_open( ) ) {
                    return rh.is_open( )
                         ? ocmp::less( lh.get( ), rh.get( ) )
                         : ocmp::less_equal( lh.get( ), rh.get( ) )
                         ;
                } else if( lh.is_closed( ) ) {
                    return rh.is_open( )
                         ? ocmp::less_equal( lh.get( ), rh.get( ) )
                         : ocmp::less( lh.get( ), rh.get( ) )
                         ;
                }
                return false;
            }

            static
            bool less( const value &lh, const domain_type &rh )
            {
                switch (lh.attr( )) {
                case attributes::MAX_INF:
                    return false;
                case attributes::MIN_INF:
                    return true;
                case attributes::CLOSE:
                    return ocmp::less( lh.get( ), rh );
                case attributes::OPEN:
                    return ocmp::less_equal( lh.get( ), rh );
                }
            }

            static
            bool less( const domain_type &lh, const value &rh )
            {
                switch (rh.attr( )) {
                case attributes::MAX_INF:
                    return true;
                case attributes::MIN_INF:
                    return false;
                case attributes::CLOSE:
                    return ocmp::less( lh, rh.get( ) );
                case attributes::OPEN:
                    return ocmp::less_equal( lh, rh.get( ) );
                }
            }

            static
            bool equal( const value &lh, const value &rh )
            {
                switch (lh.attr( )) {
                case attributes::MAX_INF:
                case attributes::MIN_INF:
                    return rh.attr( ) == lh.attr( );
                case attributes::CLOSE:
                case attributes::OPEN:
                    return (rh.attr( ) == lh.attr( ))
                        && ocmp::equal( lh.get( ), rh.get( ) );
                }
            }

            static
            bool equal( const value &lh, const domain_type &rh )
            {
                switch (lh.attr( )) {
                case attributes::MAX_INF:
                case attributes::MIN_INF:
                case attributes::OPEN:
                    return false;
                case attributes::CLOSE:
                    return ocmp::equal( lh.get( ), rh );
                }
            }

            static
            bool equal( const domain_type &lh, const value &rh  )
            {
                return equal( rh, lh );
            }
        };

    private:
        domain_type val_;
        attributes  attr_;
    };

    template <typename D, typename C>
    inline
    bool operator < ( const value<D, C> &lh, const value<D, C> &rh )
    {
        using cmp = typename value<D, C>::cmp;
        return cmp::less( lh, rh );
    }

    template <typename D, typename C>
    inline
    bool operator < ( const value<D, C> &lh, const D &rh )
    {
        using cmp = typename value<D, C>::cmp;
        return cmp::less( lh, rh );
    }

    template <typename D, typename C>
    inline
    bool operator < ( const D &lh, const value<D, C> &rh )
    {
        using cmp = typename value<D, C>::cmp;
        return cmp::less( lh, rh );
    }

    template <typename D, typename C>
    inline
    bool operator == ( const value<D, C> &lh, const value<D, C> &rh )
    {
        using cmp = typename value<D, C>::cmp;
        return cmp::equal( lh, rh );
    }

    template <typename D, typename C>
    inline
    bool operator == ( const value<D, C> &lh, const D &rh )
    {
        using cmp = typename value<D, C>::cmp;
        return cmp::equal( lh, rh );
    }

    template <typename D, typename C>
    inline
    bool operator == ( const D &lh, const value<D, C> &rh )
    {
        using cmp = typename value<D, C>::cmp;
        return cmp::equal( lh, rh );
    }

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
            break;
        }
        return o;
    }

}}

#endif // INFINITE_H
