#ifndef ETOOL_QUEUES_CONDITION_TRAITS_PRIORITY_H
#define ETOOL_QUEUES_CONDITION_TRAITS_PRIORITY_H

#include <vector>
#include <algorithm>

namespace etool { namespace queues { namespace condition {

namespace traits {

    template <typename ValueType, typename LessType = std::less<ValueType> >
    struct priority {

        typedef ValueType value_type;

        typedef std::vector<value_type> queue_type;

        static
        void push( queue_type &q, value_type value )
        {
            q.emplace_back( std::move(value) );
            std::push_heap( q.begin( ), q.end( ), LessType( ) );
        }

        static
        value_type &front( queue_type &q )
        {
            return q.front( );
        }

        static
        const value_type &front( const queue_type &q )
        {
            return q.front( );
        }

        static
        void pop( queue_type &q )
        {
            std::pop_heap( q.begin( ), q.end( ), LessType( ) );
            return q.resize( q.size( ) - 1 );
        }

        static
        typename queue_type::size_type size( const queue_type &q )
        {
            return q.size( );
        }

        static
        bool empty( const queue_type &q )
        {
            return q.empty( );
        }

        static
        void clear( queue_type &q )
        {
            q.clear( );
        }
    };

}

}}}

#endif // PRIORITY_H
