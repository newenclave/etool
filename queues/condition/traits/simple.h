#ifndef ETOOL_QUEUES_CONDITION_TRAITS_SIMPLE_H
#define ETOOL_QUEUES_CONDITION_TRAITS_SIMPLE_H

#include <queue>
#include <cstdint>

namespace etool { namespace queues { namespace condition {

namespace traits {

    template <typename ValueType>
    struct simple {

        typedef ValueType value_type;
        typedef std::queue<value_type> queue_type;

        static
        void push( queue_type &q, value_type value )
        {
            q.push( std::move(value) );
        }

//        static
//        void push( queue_type &q, const value_type &value )
//        {
//            q.push( value );
//        }

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
            return q.pop( );
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

#endif // SIMPLE_H
