#ifndef ETOOL_INTERVALS_INTERVAL_H
#define ETOOL_INTERVALS_INTERVAL_H

#include <algorithm>
#include <functional>
#include <sstream>

#include "etool/intervals/attributes.h"
#include "etool/intervals/endpoint_type.h"

namespace etool { namespace intervals {

    namespace op {

        class no_type {
            bool b[2];
            bool do_not_use( ) const { return b[0]; } /// avoid warning for
        public:
            no_type(  );
            ~no_type( );
        };

        bool check(...);
        no_type check( const no_type& );

        struct zero_ref {
        protected:
            zero_ref( );
            template <typename T>
            static const T &get( )
            {
                return *reinterpret_cast<T*>( nullptr );
            }
        };

        namespace equal {

            template<typename T, typename Arg>
            no_type operator == ( const T&, const Arg& );

            template <typename T, typename Arg = T>
            struct exists: public zero_ref {
                enum {
                    res_size = sizeof( check( get<T>( ) == get<Arg>( ) ) ),
                };
                static const bool value = ( res_size != sizeof( no_type ) );
            };

            template <typename ValueT, typename LessComparator, bool>
            struct comparator;

            /// for those who have operator ==
            template <typename ValueT, typename LessComparator>
            struct comparator<ValueT, LessComparator, true> {
                static
                bool less( const ValueT &lh, const ValueT &rh )
                {
                    const LessComparator compare;
                    return compare( lh, rh );
                }

                static
                bool equal( const ValueT &lh, const ValueT &rh )
                {
                    return lh == rh;
                }
            };

            /// for those who don't know how to compare with ==
            template <typename ValueT, typename LessComparator>
            struct comparator<ValueT, LessComparator, false> {
                static
                bool less( const ValueT &lh, const ValueT &rh )
                {
                    const LessComparator compare;
                    return compare( lh, rh );
                }

                static
                bool equal( const ValueT &lh, const ValueT &rh )
                {
                    return !less( lh, rh ) && !less( rh, lh );
                }
            };

            template <typename T, typename LessComparator>
            using cmp = comparator<T, LessComparator, exists<T>::value>;

        }

        namespace less_equal {

            template<typename T, typename Arg>
            no_type operator <= ( const T&, const Arg& );

            template <typename T, typename Arg = T>
            struct exists: public zero_ref {
                enum {
                    res_size = sizeof( check( get<T>( ) <= get<Arg>( ) ) ),
                };
                static const bool value = ( res_size != sizeof( no_type ) );
            };

            template <typename ValueT, typename LessComparator, bool>
            struct comparator;

            /// for those who have operator <=
            template <typename ValueT, typename LessComparator>
            struct comparator<ValueT, LessComparator, true> {
                static
                bool less_equal( const ValueT &lh, const ValueT &rh )
                {
                    return lh <= rh;
                }
            };

            /// for those who don't know how to compare with <=
            template <typename ValueT, typename LessComparator>
            struct comparator<ValueT, LessComparator, false> {
                static
                bool less_equal( const ValueT &lh, const ValueT &rh )
                {
                    using eq_op = equal::cmp<ValueT, LessComparator>;
                    return eq_op::less( lh, rh ) || eq_op::equal( lh, rh );
                }
            };

            template <typename T, typename LessComparator>
            using cmp = comparator<T, LessComparator, exists<T>::value>;

        }

        template <typename ValueT, typename Comparator>
        struct cmp {
            using value_type      = ValueT;
            using comparator_type = Comparator;

            static
            bool less( const value_type &lh, const value_type &rh )
            {
                using myop = equal::cmp<value_type, comparator_type>;
                return myop::less( lh, rh );
            }

            static
            bool greater( const value_type &lh, const value_type &rh )
            {
                return less( rh, lh );
            }

            static
            bool equal( const value_type &lh, const value_type &rh )
            {
                using myop = equal::cmp<value_type, comparator_type>;
                return myop::equal( lh, rh );
            }

            static
            bool less_equal( const value_type &lh, const value_type &rh )
            {
                using myop = less_equal::cmp<value_type, comparator_type>;
                return myop::less_equal( lh, rh );
            }

            static
            bool greater_equal( const value_type &lh, const value_type &rh )
            {
                return less_equal( rh, lh );
            }
        };

    }

    template <typename DomainT, typename Comparator = std::less<DomainT> >
    class interval {

    public:

        using domain_type     = DomainT;
        using comparator_type = Comparator;

    private:

        template <typename IvalT>
        static IvalT &check( IvalT &ival )
        {
#ifdef DEBUG
            if( ival.invalid( ) ) {
                throw std::logic_error( "Invalid value." );
            }
#endif
            return ival;
        }

    public:

        using left_type  = endpoint_type<endpoint_name::LEFT>;
        using right_type = endpoint_type<endpoint_name::RIGHT>;

        interval( domain_type lh, domain_type rh,
                  attributes  lf, attributes  rf )
            :left_(std::move(lh))
            ,right_(std::move(rh))
        {
            attrs_[0] = lf;
            attrs_[1] = rf;
        }

        interval( )
            :left_(domain_type( ))
            ,right_(domain_type( ))
        {
            attrs_[0] = attributes::MIN_INF;
            attrs_[1] = attributes::MIN_INF;
        }

        interval( const domain_type &val )
            :left_(val)
            ,right_(val)
        {
            attrs_[0] = attributes::CLOSE;
            attrs_[1] = attributes::CLOSE;
        }

        interval( const interval &other )
            :left_ (other.left_)
            ,right_(other.right_)
        {
            attrs_[0] = other.attrs_[0];
            attrs_[1] = other.attrs_[1];
        }

        interval( interval &&other )
            :left_(std::move(other.left_))
            ,right_(std::move(other.right_))
        {
            attrs_[0]  = other.attrs_[0];
            attrs_[1]  = other.attrs_[1];
        }

        interval& operator = ( const interval &other )
        {
            interval tmp(other);
            swap( tmp );
            return *this;
        }

        interval& operator = ( interval &&other )
        {
            interval tmp(std::move(other));
            swap( tmp );
            return *this;
        }

        ~interval( )
        { }

        const domain_type &left( ) const noexcept
        {
            return value<endpoint_name::LEFT>( );
        }

        const domain_type &right( ) const noexcept
        {
            return value<endpoint_name::RIGHT>( );
        }

        attributes left_attr( ) const noexcept
        {
            return attrs<endpoint_name::LEFT>( );
        }

        attributes right_attr( ) const noexcept
        {
            return attrs<endpoint_name::RIGHT>( );
        }

        bool left_connected( const interval &other ) const
        {
            return connected<endpoint_name::LEFT>(other);
        }

        bool right_connected( const interval &other ) const
        {
            return other.left_connected( *this );
        }

        bool contains( const domain_type &val ) const
        {
            using EPN = endpoint_name;
            if( is_plus_inf<EPN::LEFT>( ) ) {
                return false;
            }

            bool bleft = is_minus_inf<EPN::LEFT>( )
                    ||   is_greater_equal<EPN::LEFT>( val );

            if( bleft ) {
                return is_plus_inf<EPN::RIGHT>( )
                    || is_less_equal<EPN::RIGHT>( val );
            }
            return false;
        }

        bool contains_left( const interval &other ) const
        {
            return contains_side<endpoint_name::LEFT>(other);
        }

        bool contains_right( const interval &other ) const
        {
            return contains_side<endpoint_name::RIGHT>(other);
        }

        std::pair<bool, bool> contains( const interval &other ) const
        {
            using EPN = endpoint_name;
            return std::make_pair( contains_side<EPN::LEFT>(other),
                                   contains_side<EPN::RIGHT>(other) );
        }

        void swap( interval &other )
        {
            std::swap( attrs_[0], other.attrs_[0] );
            std::swap( attrs_[1], other.attrs_[1] );
            std::swap( left_,     other.left_ );
            std::swap( right_,    other.right_ );
        }

        bool not_empty( ) const
        {
            return  has_infinite( )
                 || (has_open( ) ? cmp::less( left( ), right( ) )
                                 : cmp::less_equal( left( ), right( ) ) );
                  ;
        }

        bool empty( ) const
        {
            return !has_infinite( )
                 && has_open( )
                 && cmp::equal( left( ), right( ) );
        }

        bool is_infinite( ) const
        {
            return is_minus_inf<endpoint_name::LEFT>( )
                && is_plus_inf<endpoint_name::RIGHT>( );
        }


        bool valid( ) const
        {
            using u16 = std::uint16_t;
            using A   = attributes;
            using EPN = endpoint_name;

            auto lf = static_cast<u16>(factor<EPN::LEFT>( ));
            auto rf = static_cast<u16>(factor<EPN::RIGHT>( ));

            switch ( lf ) {
            case  static_cast<u16>(A::MIN_INF):
                return true;

            case ( static_cast<u16>(A::CLOSE) | static_cast<u16>(A::OPEN) ):
                return  ( rf == static_cast<u16>(A::MAX_INF))
                   || ( ( rf == lf) && cmp::less_equal(left( ), right( )));

            case  static_cast<u16>(A::MAX_INF):
                return  ( rf == static_cast<u16>(A::MAX_INF));

            };
            return false;
        }

        bool invalid( ) const
        {
            return !valid( );
        }

        interval connect_right( const interval &to ) const
        {
            return interval( left( ), to.left( ),
                             attrs<endpoint_name::LEFT>( ),
                             to.connected_attr<endpoint_name::LEFT>( ) );
        }

        interval connect_left( const interval &to ) const
        {
            return interval( to.right( ),  right( ),
                             to.connected_attr<endpoint_name::RIGHT>( ),
                             attrs<endpoint_name::RIGHT>( ) );
        }

        void replace_left( const interval &to )
        {
            attrs_[0] = to.attrs_[0];
            left_     = to.left_;
        }

        void replace_right( const interval &to )
        {
            attrs_[1] = to.attrs_[1];
            right_    = to.right_;
        }

    public: /// factories

        static
        interval open( domain_type lh, domain_type rh )
        {
            return interval( std::move(lh),     std::move(rh),
                             attributes::OPEN,  attributes::OPEN );
        }

        static
        interval closed( domain_type lh, domain_type rh )
        {
            return interval( std::move(lh),     std::move(rh),
                             attributes::CLOSE, attributes::CLOSE );
        }

        static
        interval degenerate( domain_type lh )
        {
            return interval( lh );
        }

        static
        interval infinite(  )
        {
            return interval( domain_type( ),      domain_type( ),
                             attributes::MIN_INF, attributes::MAX_INF);
        }

        static
        interval minus_infinite(  )
        {
            return interval( domain_type( ),      domain_type( ),
                             attributes::MIN_INF, attributes::MIN_INF);
        }

        static
        interval plus_infinite(  )
        {
            return interval( domain_type( ),      domain_type( ),
                             attributes::MAX_INF, attributes::MAX_INF);
        }

        static
        interval left_open( domain_type val )
        {
            return interval( std::move(val),   domain_type( ),
                             attributes::OPEN, attributes::MAX_INF);
        }

        static
        interval left_open( domain_type lh, domain_type rh )
        {
            return interval( std::move(lh),    std::move(rh),
                             attributes::OPEN, attributes::CLOSE);
        }

        static
        interval right_open( domain_type val )
        {
            return interval( domain_type( ),      std::move(val),
                             attributes::MIN_INF, attributes::OPEN );
        }

        static
        interval right_open( domain_type lh, domain_type rh )
        {
            return interval( std::move(lh),     std::move(rh),
                             attributes::CLOSE, attributes::OPEN );
        }

        static
        interval left_closed( domain_type val )
        {
            return interval( std::move(val),    domain_type( ),
                             attributes::CLOSE, attributes::MAX_INF );
        }

        static
        interval left_closed( domain_type lh, domain_type rh )
        {
            return right_open( std::move(lh), std::move(rh) );
        }

        static
        interval right_closed( domain_type val )
        {
            return interval( domain_type( ),        std::move(val),
                             attributes::MIN_INF,   attributes::CLOSE );
        }

        static
        interval right_closed( domain_type lh, domain_type rh )
        {
            return left_open( std::move(lh), std::move(rh) );
        }

        static
        interval intersection( const interval &lh, const interval &rh )
        {
            return interval( lh.left( ),      rh.right( ),
                             lh.left_attr( ), rh.right_attr( ) );
        }

    ////// factories

        std::string to_string( ) const
        {
            std::ostringstream oss;
            out(oss);
            return oss.str( );
        }

        template <typename Out>
        Out &out( Out &o ) const
        {
            static const char lbracket[2] = { '[', '(' };
            static const char rbracket[2] = { ']', ')' };

            static const char *minf = "-inf";
            static const char *pinf = "+inf";

            if( !valid( ) ) {
                o << "(inval)";
                return o;
            }

            switch ( attrs<endpoint_name::LEFT>( ) ) {
            case attributes::CLOSE:
                o << lbracket[0] << value<endpoint_name::LEFT>( );
                break;
            case attributes::OPEN:
                o << lbracket[1] << value<endpoint_name::LEFT>( );
                break;
            case attributes::MIN_INF:
                o << lbracket[1] << minf;
                break;
            case attributes::MAX_INF:
                o << lbracket[1] << pinf;
                break;
            }

            o << ", ";

            switch ( attrs<endpoint_name::RIGHT>( ) ) {
            case attributes::CLOSE:
                o << value<endpoint_name::RIGHT>( ) << rbracket[0];
                break;
            case attributes::OPEN:
                o << value<endpoint_name::RIGHT>( ) << rbracket[1];
                break;
            case attributes::MIN_INF:
                o << minf << rbracket[1];
                break;
            case attributes::MAX_INF:
                o  << pinf << rbracket[1];
                break;
            }

            return o;
        }

        struct cmp {

            using op_cmp = op::cmp<domain_type, Comparator>;

            static
            bool less( const domain_type &lh, const domain_type &rh )
            {
                return op_cmp::less(lh, rh);
            }

            static
            bool greater( const domain_type &lh, const domain_type &rh )
            {
                return op_cmp::greater( lh, rh );
            }

            static
            bool equal( const domain_type &lh, const domain_type &rh )
            {
                return op_cmp::equal( lh, rh );
            }

            static
            bool equal( const interval &lh, const interval &rh )
            {
                return equal_side<endpoint_name::LEFT>(lh, rh)
                    && equal_side<endpoint_name::RIGHT>(lh, rh)
                     ;
            }

            static
            bool less_equal( const domain_type &lh, const domain_type &rh )
            {
                return op_cmp::less_equal( lh, rh );
            }

            static
            bool greater_equal( const domain_type &lh, const domain_type &rh )
            {
                return op_cmp::greater_equal( lh, rh );
            }

            static
            bool less_left( const interval &lh, const interval &rh )
            {
                using attr = attributes;

                auto lf = lh.factor<endpoint_name::LEFT>( );
                auto rf = rh.factor<endpoint_name::LEFT>( );

                if( (lf == rf) && ( lf == (attr::CLOSE | attr::OPEN) ) ) {
                    if( equal( lh.value<endpoint_name::LEFT>( ),
                               rh.value<endpoint_name::LEFT>( ) ) )
                    {
                        return rh.is_close<endpoint_name::LEFT>( ) <
                               lh.is_close<endpoint_name::LEFT>( );
                    } else {
                        return less(lh.value<endpoint_name::LEFT>( ),
                                    rh.value<endpoint_name::LEFT>( ));
                    }
                } else {
                    return (lf < rf);
                }
            }

            template <endpoint_name Side>
            static
            bool equal_side( const interval &lh, const interval &rh )
            {
                auto lf = lh.factor<Side>( );
                auto rf = rh.factor<Side>( );

                if( lf == rf ) {
                    if( lf == (attributes::CLOSE | attributes::OPEN) ) {
                        return  equal( lh.value<Side>( ), rh.value<Side>( ) )
                           && ( lh.is_close<Side>( ) == rh.is_close<Side>( ) );
                    }
                    return true;
                }
                return false;
            }

            static
            bool less_right( const interval &lh, const interval &rh )
            {
                using AT = attributes;
                using EP = endpoint_name;

                auto lf = lh.factor<EP::RIGHT>( );
                auto rf = rh.factor<EP::RIGHT>( );

                if( (lf == rf) && ( lf == (AT::CLOSE | AT::OPEN) ) ) {
                    if(equal(lh.value<EP::RIGHT>( ),rh.value<EP::RIGHT>( ))) {
                        return lh.is_close<EP::RIGHT>( ) <
                               rh.is_close<EP::RIGHT>( );
                    } else {
                        return less(lh.value<EP::RIGHT>( ),
                                    rh.value<EP::RIGHT>( ));
                    }
                } else {
                    return (lf < rf);
                }
            }

            static
            bool less( const interval &lh, const interval &rh )
            {
                if( equal_side<endpoint_name::LEFT>( lh, rh ) ) {
                    return less_right( lh, rh );
                } else {
                    return less_left( lh, rh );
                }
            }

            bool operator ( )( const interval &lh, const interval &rh ) const
            {
                return less( lh, rh );
            }
        };

        struct cmp_not_overlap {

        private:

            static
            bool degen( const interval &ival )
            {
                return ival.is_close<endpoint_name::LEFT>( )
                    && ival.is_close<endpoint_name::RIGHT>( )
                    && cmp::equal( ival.left( ), ival.right( ) );
            }

            static
            bool empty( const interval &ival )
            {
                return ival.empty( );
            }

        public:

            static
            bool equal_empty( const interval &lh, const interval &rh )
            {
                return (lh.empty( ) && rh.empty( ) &&
                        cmp::equal( lh.left( ), rh.left( ) ) );
            }

            static
            bool less( const interval &lh, const interval &rh )
            {

                using EN = endpoint_name;
                using AT = attributes;

                //// ===========================================
                ///                very special cases
                switch ( rh.attrs<EN::LEFT>( ) ) {
                case attributes::MAX_INF:
                    return lh.attrs<EN::RIGHT>( ) != AT::MAX_INF;
                case attributes::MIN_INF:
                    return false;
                case attributes::OPEN:
                case attributes::CLOSE:
                    break;
                }

                switch ( lh.attrs<EN::RIGHT>( ) ) {
                case attributes::MIN_INF: /// we have already checked right flag
                    return true;
                case attributes::MAX_INF:
                    return false;
                case attributes::OPEN:
                case attributes::CLOSE:
                    break;
                }

                bool empty_left  = false;
                bool empty_right = false;

                if( equal_empty( lh, rh ) ) {
                    return false;
                } else if( (empty_left = empty( lh )) && degen(rh) ) {
                    return cmp::less_equal( lh.value<EN::RIGHT>( ),
                                            rh.value<EN::LEFT>( ) );
                } else if( (empty_right = empty( rh )) && degen(lh) ) {
                    return cmp::less( lh.value<EN::RIGHT>( ),
                                      rh.value<EN::LEFT>( ) );
                }
                ////               "very special cases"
                ///  ==============================================


                bool or_empty = empty_right || empty_left;

                switch ( lh.attrs<EN::RIGHT>( ) ) {
                case attributes::OPEN:
                    return cmp::less_equal( lh.value<EN::RIGHT>( ),
                                            rh.value<EN::LEFT>( ) )
                         ;
                case attributes::CLOSE:
                    return (or_empty || rh.is_open<EN::LEFT>( ))
                         ? cmp::less_equal( lh.value<EN::RIGHT>( ),
                                            rh.value<EN::LEFT>( ) )
                         :       cmp::less( lh.value<EN::RIGHT>( ),
                                            rh.value<EN::LEFT>( ) )
                         ;
                case attributes::MIN_INF: /// we have already checked right flag
                case attributes::MAX_INF:
                    break;
                }
                return false;
            }

            bool operator ( )( const interval &lh, const interval &rh ) const
            {
                return less( lh, rh );
            }

        };

    private:

        friend struct cmp;
        friend struct cmp_not_overlap;

        template <endpoint_name Side>
        bool is_close( ) const
        {
            using S = endpoint_type<Side>;
            return ( attrs_[S::id] == attributes::CLOSE );
        }

        template <endpoint_name Side>
        bool is_open( ) const
        {
            using S = endpoint_type<Side>;
            return ( attrs_[S::id] == attributes::OPEN );
        }

        bool has_infinite( ) const
        {
            return ( ( attrs<endpoint_name::LEFT>( )
                     | attrs<endpoint_name::RIGHT>( ) )
                   & (attributes::MIN_INF | attributes::MAX_INF) ) != 0;
                 ;
        }

        bool has_open( ) const
        {
            return is_open<endpoint_name::LEFT> ( )
                || is_open<endpoint_name::RIGHT>( )
                 ;
        }

        template <endpoint_name Side>
        bool is_any_inf( ) const
        {
            return is_minus_inf<Side>( )
                || is_plus_inf<Side>( );
        }

        template <endpoint_name Side>
        bool is_minus_inf( ) const
        {
            using S = endpoint_type<Side>;
            return ( attrs_[S::id] == attributes::MIN_INF );
        }

        template <endpoint_name Side>
        bool is_plus_inf( ) const
        {
            using S = endpoint_type<Side>;
            return ( attrs_[S::id] == attributes::MAX_INF );
        }

        template <endpoint_name Side>
        const domain_type &value( ) const
        {
            using S = endpoint_type<Side>;
            return value( typename S::pointer( ) ) ;
        }

        const domain_type &value( left_type::pointer ) const
        {
            return left_;
        }

        const domain_type &value( right_type::pointer ) const
        {
            return right_;
        }

        template <endpoint_name Side>
        const attributes &attrs( ) const
        {
            using S = endpoint_type<Side>;
            return attrs_[S::id];
        }

    private:

        template <endpoint_name Side>
        domain_type &&mv_value( )
        {
            using S = endpoint_type<Side>;
            return mv_value(typename S::pointer( ));
        }

        domain_type &&mv_value( left_type::pointer )
        {
            return std::move(left_);
        }

        domain_type &&mv_value( right_type::pointer )
        {
            return std::move(right_);
        }

        domain_type &&mv_left( )
        {
            return mv_value<endpoint_name::LEFT>( );
        }

        domain_type &&mv_right( )
        {
            return mv_value<endpoint_name::RIGHT>( );
        }

        template <endpoint_name Side>
        bool is_less_equal( const domain_type &val ) const
        {
            using V = endpoint_type<Side>;
            using ptr = typename V::pointer;
            const ptr p;
            return is_close<Side>( )
                 ? cmp::less_equal( val, value( p ) )
                 : cmp::less( val, value( p ) )
                 ;
        }

        template <endpoint_name Side>
        bool is_greater_equal( const domain_type &val ) const
        {
            using V = endpoint_type<Side>;
            using ptr = typename V::pointer;
            const ptr p;
            return is_close<Side>( )
                 ? cmp::greater_equal( val, value( p ) )
                 : cmp::greater( val, value( p ) )
                 ;
        }

        template <endpoint_name Side>
        bool contains_side( const interval &other ) const
        {
            bool bleft  = false;

            if( other.is_minus_inf<Side>( ) ) {
                bleft = false; //is_minus_inf<Side>( );
            } else if( other.is_plus_inf<Side>( ) ) {
                bleft = false; //is_plus_inf<Side>( );
            } else {
                bleft = contains( other.value<Side>( ) );
                if( !bleft ) {
                    bleft = other.is_open<Side>( )
                       && is_open<Side>( )
                       && cmp::equal( value<Side>( ), other.value<Side>( ) );
                }
            }
            return bleft;
        }

        template <endpoint_name Side>
        bool connected( const interval &other ) const
        {
            using Op = typename endpoint_type<Side>::opposite;

            if( is_any_inf<Side>( ) || other.is_any_inf<Op::name>( ) ) {
                return false;
            } else {
                bool eq = cmp::equal( value<Side>( ), other.value<Op::name>( ));
                if( eq ) {
                    return empty( )
                        || other.empty( )
                        || (attrs<Side>( ) ==
                            other.connected_attr<Op::name>( ));
                }
                return false;
            }
        }

        template <endpoint_name Side>
        attributes connected_attr( ) const
        {
            auto f = attrs<Side>( );
            switch( f ){

            case attributes::CLOSE: return attributes::OPEN;
            case attributes::OPEN:  return attributes::CLOSE;

            case attributes::MIN_INF:
            case attributes::MAX_INF:
                break;
            }
            return f;
        }

        template <endpoint_name Side>
        attributes factor( ) const
        {
            auto f = attrs<Side>( );
            switch( f ) {
            case attributes::OPEN:
            case attributes::CLOSE:
                return attributes::OPEN | attributes::CLOSE;

            case attributes::MIN_INF:
            case attributes::MAX_INF:
                break;
            }
            return  f;
        }

    private:

        domain_type left_;
        domain_type right_;
        attributes attrs_[2];
    };

    template <typename ValueT, typename Comparator>
    inline
    bool operator < ( const interval<ValueT, Comparator> &lh,
                      const interval<ValueT, Comparator> &rh )
    {
        using Ival = interval<ValueT, Comparator>;
        return Ival::cmp::less( lh, rh );
    }

    template <typename ValueT, typename Comparator>
    inline
    std::ostream &operator << ( std::ostream &o,
                                const interval<ValueT, Comparator> &val )
    {
        return val.out( o );
    }

}}

#endif // INTERVAL_H
