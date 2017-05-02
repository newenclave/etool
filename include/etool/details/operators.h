#ifndef ETOOL_DETAILS_OPERATORS_H
#define ETOOL_DETAILS_OPERATORS_H

//// derived operators (if some class has only "less")

namespace etool { namespace details { namespace operators {

    class no_type {
        no_type( ) = delete;
        bool b[2];
        bool do_not_use( ) const { return b[0]; } /// avoid warinig for
    };

    bool check(...);
    no_type check( const no_type& );

    namespace equal {
        template<typename T, typename Arg>
        no_type operator == ( const T&, const Arg& );

        template <typename T, typename Arg = T>
        struct exists {
            enum { value = ( sizeof( check( *reinterpret_cast<T*>( 0 ) ==
                                            *reinterpret_cast<Arg*>( 0 ) )
                                   ) != sizeof( no_type )
                           )
                 };
        };


        template <typename ValueT, typename LessComparator, bool>
        struct cmp_value;

        /// for those who have opetator ==
        template <typename ValueT, typename LessComparator>
        struct cmp_value<ValueT, LessComparator, true> {
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
        struct cmp_value<ValueT, LessComparator, false> {
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

        template <typename ValueT, typename LessComparator>
        using cmp = cmp_value<ValueT, LessComparator,
                              exists<ValueT>::value>;

    }

    namespace less_equal {
        template<typename T, typename Arg>
        no_type operator <= ( const T&, const Arg& );

        template <typename T, typename Arg = T>
        struct exists {
            enum { value = ( sizeof( check( *reinterpret_cast<T*>( 0 ) <=
                                            *reinterpret_cast<Arg*>( 0 ) )
                                   ) != sizeof( no_type )
                           )
                 };
        };
        template <typename ValueT, typename LessComparator, bool>
        struct cmp_value;

        /// for those who have opetator <=
        template <typename ValueT, typename LessComparator>
        struct cmp_value<ValueT, LessComparator, true> {
            static
            bool less_equal( const ValueT &lh, const ValueT &rh )
            {
                return lh <= rh;
            }
        };

        /// for those who don't know how to compare with <=
        template <typename ValueT, typename LessComparator>
        struct cmp_value<ValueT, LessComparator, false> {
            static
            bool less_equal( const ValueT &lh, const ValueT &rh )
            {
                using eq_op = equal::cmp<ValueT, LessComparator>;
                return eq_op::less( lh, rh ) || eq_op::equal( lh, rh );
            }
        };

        template <typename ValueT, typename LessComparator>
        using cmp = cmp_value<ValueT, LessComparator,
                              exists<ValueT>::value>;
    }

    template <typename ValueT, typename LessComparator>
    struct cmp {
        using value_type      = ValueT;
        using comparator_type = LessComparator;

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
