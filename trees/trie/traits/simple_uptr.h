#ifndef ETOOL_TREES_TRIE_TRAITS_SIMPLE_UPTR_H
#define ETOOL_TREES_TRIE_TRAITS_SIMPLE_UPTR_H

#include <map>
#include <memory>

#if 0

namespace etool { namespace trees { namespace trie {

namespace traits {

    template <typename KeyT, typename ValueT>
    class simple_uptr {

        using key_type   = KeyT;
        using value_type = ValueT;

        using simple_uptr_ptr = std::unique_ptr<simple_uptr>;
        using nodes_map       = std::map<key_type, value_type>;

        simple_uptr( )
            :final_(false)
        { }

        simple_uptr *get( const Key &k )
        {
            auto f = next_.find( k );
            return (f == next_.end( )) ? nullptr : f->second.get( );
        }

        static
        simple_uptr create( )
        {
            return std::move(simple_uptr(new prefix_node));
        }

        prefix_node *add( const Key &k )
        {
            auto f = next_.insert( std::make_pair( k, create( ) ) );
            return f.first->second.get( );
        }

    private:

        bool        final_;
        Value       val_;
        nodes_map   next_;

    };
}


}}}

#endif

#endif
