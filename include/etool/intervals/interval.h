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
            bool _( ) const { return b[0]; }
        };

        template<typename T, typename Arg>
        no_type operator == ( const T&, const Arg& );

        bool check(...);
        no_type& check( const no_type& );

        template <typename T, typename Arg = T>
        struct equal_exists {
            enum { value = ( sizeof( check( *reinterpret_cast<T*>(0) ==
                                            *reinterpret_cast<Arg*>(0) )
                                   ) != sizeof( no_type )
                           )
                 };
        };

        template <typename ValueT, typename Comparator, bool>
        struct cmp_value;

        template <typename ValueT, typename Comparator>
        struct cmp_value<ValueT, Comparator, true> {
            static
            bool less( const ValueT &lh, const ValueT &rh )
            {
                const Comparator compare;
                return compare( lh, rh );
            }

            static
            bool equal( const ValueT &lh, const ValueT &rh )
            {
                return lh == rh;
            }
        };

        template <typename ValueT, typename Comparator>
        struct cmp_value<ValueT, Comparator, false> {
            static
            bool less( const ValueT &lh, const ValueT &rh )
            {
                const Comparator compare;
                return compare( lh, rh );
            }

            static
            bool equal( const ValueT &lh, const ValueT &rh )
            {
                return !less( lh, rh ) && !less( rh, lh );
            }
        };

        template <typename ValueT, typename Comparator>
        using cmp = cmp_value< ValueT,
                               Comparator,
                               equal_exists<ValueT>::value>;

    }

    template <typename ValueT, typename Comparator = std::less<ValueT> >
    class interval {

    public:

        using value_type      = ValueT;
        using comparator_type = Comparator;

    private:

        template <typename IvalT>
        static IvalT &check( IvalT &ival )
        {
#ifdef DEBUG
            if( !ival.valid( ) ) {
                throw std::logic_error( "Invalid value." );
            }
#endif
            return ival;
        }

        interval( value_type lh, value_type rh,
                  attributes lf, attributes rf )
            :left_(std::move(lh))
            ,right_(std::move(rh))
        {
            attrs_[0] = lf;
            attrs_[1] = rf;
        }

        template <attributes Flag = attributes::CLOSE>
        interval( endpoint_type<value_type, endpoint_name::LEFT> side,
                  attribute<Flag> sf = attribute<Flag>( ) )
            :left_(std::move(side.move( ) ))
        {
            using my_side = endpoint_type<value_type, endpoint_name::LEFT>;
            using op_side = typename my_side::opposite;

            attrs_[my_side::id] = sf.get( );
            attrs_[op_side::id] = op_side::def_inf;
        }

        template <attributes Flag = attributes::CLOSE>
        interval( endpoint_type<value_type, endpoint_name::RIGHT> side,
                  attribute<Flag> sf = attribute<Flag>( ) )
            :right_(std::move(side.move( ) ))
        {
            using my_side = endpoint_type<value_type, endpoint_name::RIGHT>;
            using op_side = typename my_side::opposite;

            attrs_[my_side::id] = sf.get( );
            attrs_[op_side::id] = op_side::def_inf;
        }

        template <attributes LFlag,     attributes RFlag>
        interval( endpoint_type<value_type, endpoint_name::LEFT>  ls,
                  endpoint_type<value_type, endpoint_name::RIGHT> rs,
                  attribute<LFlag> lf,  attribute<RFlag>  rf)
            :left_(ls.move( ))
            ,right_(rs.move( ))
        {
            using my_side = endpoint_type<value_type, endpoint_name::LEFT>;
            using op_side = endpoint_type<value_type, endpoint_name::RIGHT>;

            attrs_[my_side::id] = lf.get( );
            attrs_[op_side::id] = rf.get( );
        }

        template <attributes LInf = attributes::MIN_INF,
                  attributes RInf = attributes::MAX_INF>
        interval( typename endpoint_type<value_type,
                           endpoint_name::LEFT>::template inf<LInf> ls,
                  typename endpoint_type<value_type,
                           endpoint_name::RIGHT>::template inf<RInf> rs)
        {
            static_assert( LInf <= RInf, "Left side must be less then right" );

            using my_side = endpoint_type<value_type, endpoint_name::LEFT>;
            using op_side = endpoint_type<value_type, endpoint_name::RIGHT>;

            attrs_[my_side::id] = ls.get( );
            attrs_[op_side::id] = rs.get( );
        }

    public:

        using left_type  = endpoint_type<value_type, endpoint_name::LEFT>;
        using right_type = endpoint_type<value_type, endpoint_name::RIGHT>;

        interval( )
        {
            attrs_[0] = attributes::MIN_INF;
            attrs_[1] = attributes::MIN_INF;
        }

        interval( const ValueT &val )
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

        const value_type &left( ) const noexcept
        {
            return value<endpoint_name::LEFT>( );
        }

        const value_type &right( ) const noexcept
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

        bool contains( const value_type &val ) const
        {
            bool bleft = is_minus_inf<endpoint_name::LEFT>( )
                    ||   is_greater_equal<endpoint_name::LEFT>( val );

            if( bleft ) {
                return is_plus_inf<endpoint_name::RIGHT>( )
                    || is_less_equal<endpoint_name::RIGHT>( val );
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
            return std::make_pair( contains_side<endpoint_name::LEFT>(other),
                                   contains_side<endpoint_name::RIGHT>(other) );
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

        bool valid( ) const
        {
            using u16 = std::uint16_t;
            auto lf = static_cast<u16>(factor<endpoint_name::LEFT>( ));
            auto rf = static_cast<u16>(factor<endpoint_name::RIGHT>( ));

            switch ( lf ) {
            case  static_cast<u16>(attributes::MIN_INF):
                return true;
            case ( static_cast<u16>(attributes::CLOSE) |
                   static_cast<u16>(attributes::OPEN) ):
                return (rf == static_cast<u16>(attributes::MAX_INF))
                    || ((rf == lf) && cmp::less_equal(left( ), right( )));
            case  static_cast<u16>(attributes::MAX_INF):
                return (rf == static_cast<u16>(attributes::MAX_INF));
            };
            return false;
        }

        bool invalid( ) const
        {
            return !valid( );
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
        interval open( value_type lh, value_type rh )
        {
            return interval( left_type(std::move(lh)),
                             right_type(std::move(rh)),
                             attribute<attributes::OPEN>( ),
                             attribute<attributes::OPEN>( ) );
        }

        static
        interval closed( value_type lh, value_type rh )
        {
            return interval( left_type(std::move(lh)),
                             right_type(std::move(rh)),
                             attribute<attributes::CLOSE>( ),
                             attribute<attributes::CLOSE>( ) );
        }

        static
        interval degenerate( value_type lh )
        {
            return interval( lh );
        }

        static
        interval infinite(  )
        {
            return interval( typename left_type::default_inf( ),
                             typename right_type::default_inf( ) );
        }

        static
        interval minus_infinite(  )
        {
            return interval( typename left_type::template
                                        inf<attributes::MIN_INF>( ),
                             typename right_type::template
                                        inf<attributes::MIN_INF>( ) );
        }

        static
        interval plus_infinite(  )
        {
            return interval( typename left_type::template
                                        inf<attributes::MAX_INF>( ),
                             typename right_type::template
                                        inf<attributes::MAX_INF>( ) );
        }

        static
        interval left_open( value_type val )
        {
            return interval( left_type(std::move(val)),
                             attribute<attributes::OPEN>( ) );
        }

        static
        interval left_open( value_type lh, value_type rh )
        {
            return interval( left_type(std::move(lh)),
                             right_type(std::move(rh)),
                             attribute<attributes::OPEN>( ),
                             attribute<attributes::CLOSE>( ));
        }

        static
        interval right_open( value_type val )
        {
            return interval( right_type(std::move(val)),
                             attribute<attributes::OPEN>( ) );
        }

        static
        interval right_open( value_type lh, value_type rh )
        {
            return interval( left_type(std::move(lh)),
                             right_type(std::move(rh)),
                             attribute<attributes::CLOSE>( ),
                             attribute<attributes::OPEN>( ));
        }

        static
        interval left_closed( value_type val )
        {
            return interval( left_type(std::move(val)),
                             attribute<attributes::CLOSE>( ) );
        }

        static
        interval left_closed( value_type lh, value_type rh )
        {
            return right_open( std::move(lh), std::move(rh) );
        }

        static
        interval right_closed( value_type val )
        {
            return interval( right_type(std::move(val)),
                             attribute<attributes::CLOSE>( ) );
        }

        static
        interval right_closed( value_type lh, value_type rh )
        {
            return left_open( std::move(lh), std::move(rh) );
        }

        static
        interval intersection( const interval &lh, const interval &rh )
        {
            return interval( lh.left( ), rh.right( ),
                             lh.attrs<endpoint_name::LEFT>( ),
                             rh.attrs<endpoint_name::RIGHT>( ) );
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

            using op_cmp = op::cmp<value_type, Comparator>;

            static
            bool less( const value_type &lh, const value_type &rh )
            {
                return op_cmp::less(lh, rh);
            }

            static
            bool greater( const value_type &lh, const value_type &rh )
            {
                return less( rh, lh );
            }

            static
            bool equal( const value_type &lh, const value_type &rh )
            {
                return op_cmp::equal( lh, rh );
            }

            static
            bool equal( const interval &lh, const interval &rh )
            {
                return cmp::template equal_side<endpoint_name::LEFT>(lh, rh)
                    && cmp::template equal_side<endpoint_name::RIGHT>(lh, rh)
                     ;
            }

            static
            bool less_equal( const value_type &lh, const value_type &rh )
            {
                return less(lh, rh) || equal(lh, rh);
            }

            static
            bool greater_equal( const value_type &lh, const value_type &rh )
            {
                return greater( lh, rh ) || equal( lh, rh );
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

                //// very special cases
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

                //// end of "very special cases"

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
            using S = endpoint_type<value_type, Side>;
            return ( attrs_[S::id] == attributes::CLOSE );
        }

        template <endpoint_name Side>
        bool is_open( ) const
        {
            using S = endpoint_type<value_type, Side>;
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
            using S = endpoint_type<value_type, Side>;
            return ( attrs_[S::id] == attributes::MIN_INF );
        }

        template <endpoint_name Side>
        bool is_plus_inf( ) const
        {
            using S = endpoint_type<value_type, Side>;
            return ( attrs_[S::id] == attributes::MAX_INF );
        }

        template <endpoint_name Side>
        const value_type &value( ) const
        {
            using S = endpoint_type<int, Side>;
            return value( typename S::pointer( ) ) ;
        }

        const value_type &value( endpoint_type<int,
                                 endpoint_name::LEFT>::pointer ) const
        {
            return left_;
        }

        const value_type &value( endpoint_type<int,
                                 endpoint_name::RIGHT>::pointer ) const
        {
            return right_;
        }

        template <endpoint_name Side>
        const attributes &attrs( ) const
        {
            using S = endpoint_type<value_type, Side>;
            return attrs_[S::id];
        }

    private:

        template <endpoint_name Side>
        value_type &&mv_value( )
        {
            using S = endpoint_type<int, Side>;
            return mv_value(typename S::pointer( ));
        }

        value_type &&mv_value( endpoint_type<int,
                               endpoint_name::LEFT>::pointer )
        {
            return std::move(left_);
        }

        value_type &&mv_value( endpoint_type<int,
                               endpoint_name::RIGHT>::pointer )
        {
            return std::move(right_);
        }

        value_type &&mv_left( )
        {
            return mv_value<endpoint_name::LEFT>( );
        }

        value_type &&mv_right( )
        {
            return mv_value<endpoint_name::RIGHT>( );
        }

        template <endpoint_name Side>
        bool is_less_equal( const value_type &val ) const
        {
            using V = endpoint_type<int, Side>;
            using ptr = typename V::pointer;
            const ptr p;
            return is_close<Side>( )
                 ? cmp::less_equal( val, value( p ) )
                 : cmp::less( val, value( p ) )
                 ;
        }

        template <endpoint_name Side>
        bool is_greater_equal( const value_type &val ) const
        {
            using V = endpoint_type<int, Side>;
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
            using Opp = typename endpoint_type<value_type, Side>::opposite;

            if( is_any_inf<Side>( ) || other.is_any_inf<Opp::name>( ) ) {
                return false;
            } else {
                bool eq = cmp::equal( value<Side>( ),
                                      other.value<Opp::name>( ) );
                if( eq ) {
                    return empty( )
                        || other.empty( )
                        || (attrs<Side>( ) ==
                            other.connected_attr<Opp::name>( ));
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

    //    template <endpoint_name Side>
    //    std::uint16_t &flags( )
    //    {
    //        using S = endpoint_type<value_type, Side>;
    //        return flags_[S::id];
    //    }

    private:
        value_type left_ { };
        value_type right_{ };
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
