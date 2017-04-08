#ifndef ETOOL_TREES_TRIE_BASE_H
#define ETOOL_TREES_TRIE_BASE_H

#include "etool/trees/trie/nodes/simple_uptr.h"

namespace etool { namespace trees { namespace trie {

    template <typename KeyT, typename ValueT,
              template <typename, typename> class NodeT = nodes::simple_uptr>
    class base {

        using node_type = NodeT<KeyT, ValueT>;

    public:

        using key_type   = KeyT;
        using value_type = ValueT;

        base( ) = default;
        base( const base & ) = default;
        base& operator = ( const base & ) = default;
        base( base && ) = default;
        base& operator = ( base && ) = default;

        template <typename Iter>
        class result_iterator {
        public:

            ~result_iterator( ) = default;
            result_iterator ( const result_iterator &  ) = default;
            result_iterator (       result_iterator && ) = default;
            result_iterator & operator = ( const result_iterator &  ) = default;
            result_iterator & operator = (       result_iterator && ) = default;

            result_iterator( )
                :node_(nullptr)
            { }

            result_iterator( node_type *node, Iter itr )
                :node_(node)
                ,iter_(itr)
            { }

            operator bool ( ) const
            {
                return node_ != nullptr;
            }

            value_type &operator *( )
            {
                return node_->value( );
            }

            const value_type &operator *( ) const
            {
                return node_->value( );
            }

            value_type *operator -> ( )
            {
                return &node_->value( );
            }

            const value_type *operator -> ( ) const
            {
                return *node_->value( );
            }

            Iter iterator( ) const
            {
                return iter_;
            }

        private:
            node_type *node_;
            Iter       iter_;
        };

        template <typename IterT>
        void insert( IterT begin, const IterT &end, value_type value )
        {
            node_type *last = &root_;
            for( ;begin != end; ++begin ) {
                last = last->add( *begin );
            }
            last->set_final(true);
            last->set_value(std::move(value));
        }

        template <typename ContainerT>
        void insert( const ContainerT &cnt, value_type value )
        {
            insert( std::begin(cnt), std::end(cnt), std::move(value) );
        }

        void insert( const key_type *ptr, size_t len, value_type value )
        {
            insert( ptr, ptr + len, std::move(value) );
        }

        template <typename IterT>
        result_iterator<IterT> get( IterT b, const IterT &e, bool greedy )
        {
            using result_type = result_iterator<IterT>;

            node_type *next_table = &root_;
            node_type *last_final = nullptr;

            if( b == e ) {
                return result_type(nullptr, e);
            }

            IterT bb = b;
            ++bb;

            for( ; b!=e; ++b ) {

                next_table = next_table->get( *b );

                if( !next_table ) {
                    break;
                }

                if( next_table->final( ) ) {

                    last_final = next_table;

                    bb = b;
                    ++bb;

                    if( !greedy ) {
                        break;
                    }
                }
            }

            b = bb;
            return last_final ? result_type(last_final, bb)
                              : result_type(nullptr, e);
        }

    private:

        node_type root_;

    };


}}}

#endif
