#ifndef ETOOL_TREES_TRIE_NODES_MAP_H
#define ETOOL_TREES_TRIE_NODES_MAP_H

#include <map>
#include <memory>

namespace etool { namespace trees { namespace trie {

namespace nodes {

    template <typename KeyT, typename ValueT>
    class map {

    public:

        using key_type   = KeyT;
        using value_type = ValueT;
        using value_ptr  = std::unique_ptr<value_type>;

        using nodes_type = std::map<key_type, map>;

        map *get( const key_type &k )
        {
            auto f = next_.find( k );
            return (f == next_.end( )) ? nullptr : &f->second;
        }

        map *set( const key_type &k )
        {
            auto f = next_.insert( std::make_pair( k, map( ) ) );
            return &f.first->second;
        }

        void set_value( value_type val )
        {
            val_.reset( new value_type(std::move(val) ) );
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

#endif
