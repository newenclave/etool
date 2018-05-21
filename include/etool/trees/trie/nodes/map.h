#ifndef ETOOL_TREES_TRIE_NODES_MAP_H
#define ETOOL_TREES_TRIE_NODES_MAP_H

#include <map>
#include <memory>

namespace etool { namespace trees { namespace trie {

namespace nodes {

    template <typename KeyT, typename ValueT, typename Comp = std::less<KeyT> >
    class map {

    public:

        using key_type   = KeyT;
        using value_type = ValueT;
        using value_ptr  = std::unique_ptr<value_type>;

		using nodes_type = std::map<key_type, map, Comp>;
		using iterator = typename nodes_type::iterator;
		using const_iterator = typename nodes_type::const_iterator;

		void erase(const_iterator itr)
		{
			next_.erase(itr);
		}

		bool is_end(const_iterator itr) const
		{
			return itr == next_.end();
		}

		map *get_node(iterator itr)
		{
			return &itr->second;
		}

		const map *get_node(const_iterator itr) const
		{
			return &itr->second;
		}

		iterator get(const key_type &k)
		{
			return next_.find(k);
		}

		const_iterator get(const key_type &k) const
		{
			return next_.find(k);
		}

        iterator set( const key_type &k )
        {
            auto f = next_.insert( std::make_pair( k, map( ) ) );
            return f.first;
        }

        void set_value( value_type val )
        {
            val_.reset( new value_type(std::move(val) ) );
        }

		std::size_t remove( const map *node )
		{
			for (auto b = next_.begin(); b != next_.end(); ++b) {
				if (&b->second == node) {
					next_.erase(b);
					return 1;
				}
			}
			return 0;
		}

		void reset_value()
		{
			val_.reset();
		}

		bool empty() const 
		{
			return next_.empty();
		}

        value_type *value( )
        {
            return val_.get( );
        }

        const value_type *value( ) const
        {
            return val_.get( );
        }

    private:
        value_ptr  val_;
        nodes_type  next_;
    };
}


}}}

#endif /// ETOOL_TREES_TRIE_NODES_MAP_H
