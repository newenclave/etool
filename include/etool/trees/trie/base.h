#ifndef ETOOL_TREES_TRIE_BASE_H
#define ETOOL_TREES_TRIE_BASE_H

#include <stack>

#include "etool/trees/trie/nodes/map.h"
#include "etool/trees/trie/nodes/array.h"

namespace etool { namespace trees { namespace trie {

    template <typename KeyT, typename ValueT, typename Comp = std::less<KeyT>,
              template <typename,
                        typename,
                        typename> class NodeT = nodes::array>
    class base {
    public:
        using key_type   = KeyT;
        using value_type = ValueT;
        using node_type  = NodeT<KeyT, ValueT, Comp>;

    public:
        base( ) = default;
        base( const base & ) = default;
        base& operator = ( const base & ) = default;
        base( base && ) = default;
        base& operator = ( base && ) = default;

		template <typename Iter>
		class const_result_view {
		public:

			~const_result_view( ) = default;
			const_result_view(const const_result_view &) = default;
			const_result_view(const_result_view &&) = default;
			const_result_view &operator = (const const_result_view &) = default;
			const_result_view &operator = (const_result_view &&) = default;
			const_result_view( ) = default;

			const_result_view(const node_type *node, Iter itrbegin, Iter itrend)
				:node_(node)
				,begin_(itrbegin)
				,end_(itrend)
			{ }

			operator bool( ) const
			{
				return node_ != nullptr;
			}

			const value_type &operator *( ) const
			{
				return *node_->value();
			}

			const value_type *operator ->( ) const
			{
				return node_->value( );
			}

			Iter begin() const
			{
				return begin_;
			}

			Iter end() const
			{
				return end_;
			}

		private:
			const node_type *node_ = nullptr;
			Iter             begin_;
			Iter             end_;
		};

        template <typename Iter>
        class result_view {
        public:

            ~result_view( ) = default;
            result_view ( const result_view &  ) = default;
            result_view (       result_view && ) = default;
            result_view & operator = ( const result_view &  ) = default;
            result_view & operator = (       result_view && ) = default;

            result_view( )
                :node_(nullptr)
            { }

            result_view( node_type *node, Iter itrbegin, Iter itrend )
                :node_(node)
                ,begin_(itrbegin)
                ,end_(itrend)
            { }

            operator bool ( ) const
            {
                return node_ != nullptr;
            }

            value_type &operator *( )
            {
                return *node_->value( );
            }

            const value_type &operator *( ) const
            {
                return *node_->value( );
            }

            value_type *operator -> ( )
            {
                return node_->value( );
            }

            const value_type *operator -> ( ) const
            {
                return node_->value( );
            }

            Iter begin( ) const
            {
                return begin_;
            }

            Iter end( ) const
            {
                return end_;
            }

        private:
            node_type *node_ = nullptr;
            Iter       begin_;
            Iter       end_;
        };

        template <typename IterT>
        void set( IterT begin, const IterT &end, value_type value )
        {
            set_s( &root_, begin, end, std::move(value) );
        }

        template <typename ContainerT>
        void set( const ContainerT &cnt, value_type value )
        {
            set( std::begin(cnt), std::end(cnt), std::move(value) );
        }

        void set( const key_type *ptr, size_t len, value_type value )
        {
            set( ptr, ptr + len, std::move(value) );
        }

		template <typename IterT>
		result_view<IterT> get(IterT b, const IterT &e, bool greedy)
		{
			return get_s<result_view>(&root_, b, e, greedy);
		}

		template <typename IterT>
		const_result_view<IterT> get(IterT b, const IterT &e, bool greedy) const
		{
			return get_s<const_result_view>(&root_, b, e, greedy);
		}

		using node_stack = std::stack<node_type *>;

		template <typename IterT>
		std::size_t remove(IterT b, const IterT &e)
		{
			//auto stack = get_node_stack(b, e);
			//while(!stack.empty()) {
			//	node_type *top = stack.top();
			//	top->reset_value();
			//	if (top->empty()) {

			//	}
			//}
			return 0;
		}

		template <typename IterT>
		node_stack get_node_stack(IterT begin, const IterT &end) 
		{
			return get_node_stack(&root_, begin, end);
		}

		template <typename IterT>
		static
		node_stack get_node_stack(node_type *node, IterT begin, const IterT &end)
		{
			node_stack result;
			if (begin == end) {
				return node_stack();
			}

			for (; begin != end; ++begin) {
				result.push(node);
				node = node->get(*begin);
				if (!node) {
					break;
				}
			}
			if (node && node->value()) {
				result.push(node);
				return result;
			}
			return node_stack();
		}

	private:

        template <typename IterT>
        static
        void set_s( node_type *last,
                    IterT begin, const IterT &end, value_type value )
        {
            for( ;begin != end; ++begin ) {
                last = last->set( *begin );
            }
            last->set_value(std::move(value));
        }

        template <template <typename> class ResultType, 
                  typename IterT, typename NodeType>
        static
        ResultType<IterT> get_s( NodeType next_table,
                                 IterT b, const IterT &e, bool greedy )
        {
            using result_type = ResultType<IterT>;

			NodeType last_final = nullptr;

            if( b == e ) {
                return result_type(nullptr, b, e);
            }

			IterT start = b;
            IterT bb = b;
            ++bb;

            for( ; b!=e; ++b ) {

                next_table = next_table->get( *b );

                if( !next_table ) {
                    break;
                }

                if( next_table->value( ) ) {

                    last_final = next_table;

                    bb = b;
                    ++bb;

                    if( !greedy ) {
                        break;
                    }
                }
            }

            return last_final ? result_type(last_final, start, bb)
                              : result_type(nullptr, e, e);
        }

        node_type root_;

    };


}}}

#endif
