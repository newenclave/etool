#ifndef ETOOL_INTERVALS_OPERATORS_H
#define ETOOL_INTERVALS_OPERATORS_H

namespace etool { namespace intervals { namespace operators {

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
        static const T &get( );
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
                LessComparator compare;
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
}}}

#endif // OPERATORS_H
